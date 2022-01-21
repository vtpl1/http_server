// *****************************************************
//  Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <Poco/ErrorHandler.h>
#include <Poco/Net/Net.h>
#include <Poco/Util/AbstractConfiguration.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/Util/ServerApplication.h>

#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include "command_receiver.h"
#include "end_point_manager.h"
#include "job_list_manager.h"
#include "job_to_media_command_mapper.h"
#include "logging.h"
#include "pipeline.h"
#include "pipeline_manager.h"

constexpr int http_server_default_port = 8080;
class ServerErrorHandler : public Poco::ErrorHandler
{
public:
  void exception(const Poco::Exception& exc) override { RAY_LOG_ERR << "Poco::Exception " << exc.what(); }
  void exception(const std::exception& exc) override { RAY_LOG_ERR << "std::exception " << exc.what(); }
  void exception() override { RAY_LOG_ERR << "unknown exception "; }
};

class EntryPoint : public Poco::Util::ServerApplication
{
private:
  std::string _session_dir{};
  ServerErrorHandler _serverErrorHandler;

  bool _help_requested{false};
  std::string _name_of_app{};
  // static std::atomic_bool do_shutdown;

public:

  EntryPoint() { setUnixOptions(true); }
  void initialize(Application& self) override
  {
    loadConfiguration(); // load default configuration files, if present
    Application::initialize(self);
    Poco::Net::initializeNetwork();
    Poco::ErrorHandler::set(&_serverErrorHandler);
    std::string data_dir;
    if (data_dir.empty()) {
      data_dir = get_session_folder();
    }
    _name_of_app = config().getString("application.baseName");
    std::cout << "Session folder at: " << data_dir << std::endl;
    std::cout << "Log at: " << data_dir << _name_of_app << "_" << config().getString("system.pid") << ".log"
              << std::endl;
    ::ray::RayLog::GetLoggerName();
    ::ray::RayLog::StartRayLog(_name_of_app, ::ray::RayLogLevel::INFO, data_dir);
  }

  void uninitialize() override
  {
    Poco::Net::uninitializeNetwork();
    Application::uninitialize();
  }

  void handleHelp(const std::string& /*name*/, const std::string& /*value*/)
  {
    _help_requested = true;
    displayHelp();
    stopOptionsProcessing();
  }

  void printProperties(const std::string& base)
  {
    Poco::Util::AbstractConfiguration::Keys keys;
    config().keys(base, keys);
    if (keys.empty()) {
      if (config().hasProperty(base)) {
        std::string msg;
        msg.append(base);
        msg.append(" = ");
        msg.append(config().getString(base));
        logger().information(msg);
      }
    } else {
      for (const auto& key : keys) {
        std::string fullKey = base;
        if (!fullKey.empty()) {
          fullKey += '.';
        }
        fullKey.append(key);
        printProperties(fullKey); // NOLINT
      }
    }
  }

  void defineOptions(Poco::Util::OptionSet& options) override
  {
    Poco::Util::ServerApplication::defineOptions(options);
    options.addOption(Poco::Util::Option("help", "h", "display help information on command line arguments")
                          .required(false)
                          .repeatable(false)
                          .callback(Poco::Util::OptionCallback<EntryPoint>(this, &EntryPoint::handleHelp)));
    options.addOption(Poco::Util::Option("config-file", "f", "load configuration data from a file")
                          .required(false)
                          .repeatable(true)
                          .argument("file")
                          .callback(Poco::Util::OptionCallback<EntryPoint>(this, &EntryPoint::handleConfig)));
    options.addOption(Poco::Util::Option("mode", "m", "select the mode for job")
                          .required(false)
                          .repeatable(false)
                          .argument("value")
                          .binding("mode"));
    options.addOption(Poco::Util::Option("port", "p", "Supply server port")
                          .required(false)
                          .repeatable(false)
                          .argument("value")
                          .binding("server_port"));
  }

  void handleConfig(const std::string& name, const std::string& value) { loadConfiguration(value); }

  void handleOption(const std::string& name, const std::string& value) override
  {
    std::cout << "name : [" << name << "] value: [" << value << "]" << std::endl;
    // if (name == "help") {
    //   _help_requested = true;
    // } else if (name == "input") {
    //   _input_url = value;
    // } else if (name == "output") {
    //   _output_url = value;
    // } else if (name == "timeout") {
    //   _open_or_listen_timeout_in_sec = std::stoi(value);
    // }
    Application::handleOption(name, value);
  }

  void displayHelp()
  {
    Poco::Util::HelpFormatter helpFormatter(options());
    helpFormatter.setCommand(commandName());
    helpFormatter.setUsage("OPTIONS");
    helpFormatter.setHeader("Media handler class.");
    helpFormatter.format(std::cout);
  }

  const std::string& get_session_folder()
  {
    if (_session_dir.empty()) {
      Poco::Path base_path;
      if (config().getBool("application.runAsService", false) || config().getBool("application.runAsDaemon", false)) {
        base_path.assign(config().getString("application.dataDir", config().getString("application.dir", "./")));
        base_path.pushDirectory(config().getString("application.baseName", "ojana"));
      } else {
        base_path.assign(config().getString("application.dir", "./"));
      }
      base_path.pushDirectory("session");
      _session_dir = base_path.toString();
    }
    return _session_dir;
  }

  static void sigHandlerAppClose(int l_signo)
  {
    if (l_signo == SIGINT) {
      RAY_LOG(WARNING) << "Signal: " << l_signo << ":SIGINT(Interactive attention signal) received.\n";
      // do_shutdown = true;
    }
  }

  int main(const ArgVec& args) final
  {
    if (_help_requested) {
      return Application::EXIT_OK;
    }
    //::ray::RayLog::StartRayLog(name_of_app, ::ray::RayLogLevel::DEBUG, get_session_folder());
    if (signal(SIGINT, EntryPoint::sigHandlerAppClose) == SIG_ERR) { // NOLINT
      RAY_LOG_FAT << "Can't attach sigHandlerAppClose signal\n";
      return ExitCode::EXIT_OSERR;
    }
    RAY_LOG(INFO) << "main Started: " << _name_of_app;
    // printProperties("");
    std::string job_mode = config().getString("mode", "server");
    int server_port = config().getInt("server_port", http_server_default_port);
    if (job_mode == "server") {
      {
        JobToMediaCommandMapper::set_base_dir(get_session_folder());
        JobToMediaCommandMapper::set_file_name("server.yaml");
        std::unique_ptr<EndPointManager> epm = std::make_unique<EndPointManager>(
            JobListManager::get_instance(), config().getString("system.currentDir"), server_port);
        std::unique_ptr<PipelineManager> plm =
            std::make_unique<PipelineManager>(JobListManager::get_instance(), config().getString("system.currentDir"));
        epm->start();
        plm->start();
        RAY_LOG_INF << "Server started";
        waitForTerminationRequest();
        RAY_LOG_INF << "Server stop request received";
        plm->stop();
        epm->stop();
      }
      RAY_LOG_INF << "Server stopped";
    } else if (job_mode == "client") {
      {
        JobToMediaCommandMapper::set_base_dir(get_session_folder());
        JobToMediaCommandMapper::set_file_name("client.yaml");
        std::unique_ptr<CommandReceiver> cmdr =
            std::make_unique<CommandReceiver>("localhost", server_port, JobListManager::get_instance());
        std::unique_ptr<PipelineManager> plm =
            std::make_unique<PipelineManager>(JobListManager::get_instance(), config().getString("system.currentDir"));
        cmdr->start();
        plm->start();
        RAY_LOG_INF << "Client started";
        waitForTerminationRequest();
        RAY_LOG_INF << "Client stop request received";
        plm->stop();
        cmdr->stop();
      }
      RAY_LOG_INF << "Client stopped";
    } else {
      RAY_LOG_ERR << "invalid job_mode";
    }

    return Application::EXIT_OK;
  }

  // To run the http server
  // .\HttpServer.exe

  // To run the rtmp to hls
  // .\build\entrypoint\Debug\media_converter.exe -i rtmp://0.0.0.0:9001 -o ./videos/play.m3u8

  // To run the rtsp to rtmp
  // .\build\entrypoint\Debug\media_converter.exe -i rtsp://admin:AdmiN1234@192.168.0.58/h264/ch1/main/ -o
  // rtmp://localhost:9001

};

// std::atomic_bool EntryPoint::do_shutdown{false};

POCO_SERVER_MAIN(EntryPoint);
