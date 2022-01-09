#include <Poco/ErrorHandler.h>
#include <Poco/Net/Net.h>
#include <Poco/Util/AbstractConfiguration.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/Util/ServerApplication.h>
#include <httplib.h>

#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include "logging.h"

class ServerErrorHandler : public Poco::ErrorHandler
{
public:
  void exception(const Poco::Exception& exc) override { RAY_LOG(FATAL) << "Poco::Exception " << exc.what(); }
  void exception(const std::exception& exc) override { RAY_LOG(FATAL) << "std::exception " << exc.what(); }
  void exception() override { RAY_LOG(FATAL) << "unknown exception "; }
};

class EntryPoint : public Poco::Util::ServerApplication
{
private:
  std::string _session_dir{};
  ServerErrorHandler _serverErrorHandler;

  bool _help_requested{false};
  std::string _input_url{};
  std::string _output_url{};
  int _open_or_listen_timeout_in_sec{10};
  std::string _name_of_app{};

public:
  static std::atomic_bool do_shutdown;
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

  void handleHelp(const std::string& name, const std::string& value)
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
      for (Poco::Util::AbstractConfiguration::Keys::const_iterator it = keys.begin(); it != keys.end(); ++it) {
        std::string fullKey = base;
        if (!fullKey.empty())
          fullKey += '.';
        fullKey.append(*it);
        printProperties(fullKey);
      }
    }
  }

  void defineOptions(Poco::Util::OptionSet& options) override
  {
    Application::defineOptions(options);
    options.addOption(Poco::Util::Option("help", "h", "display help information on command line arguments")
                          .required(false)
                          .repeatable(false)
                          .callback(Poco::Util::OptionCallback<EntryPoint>(this, &EntryPoint::handleHelp)));
    options.addOption(Poco::Util::Option("config-file", "f", "load configuration data from a file")
                          .required(false)
                          .repeatable(true)
                          .argument("file")
                          .callback(Poco::Util::OptionCallback<EntryPoint>(this, &EntryPoint::handleConfig)));
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
      do_shutdown = true;
    }
  }

  int main(const ArgVec& args)
  {
    if (_help_requested) {
      return Application::EXIT_OK;
    }
    //::ray::RayLog::StartRayLog(name_of_app, ::ray::RayLogLevel::DEBUG, get_session_folder());
    if (signal(SIGINT, EntryPoint::sigHandlerAppClose) == SIG_ERR) {
      RAY_LOG(FATAL) << "Can't attach sigHandlerAppClose signal\n";
      return ExitCode::EXIT_OSERR;
    }
    RAY_LOG(INFO) << "main Started: " << _name_of_app;
    printProperties("");
    RAY_LOG_INF << "Starting with input : " << _input_url << " : output : " << _output_url;
    std::unique_ptr<httplib::Server> svr = std::make_unique<httplib::Server>();
    svr->listen("localhost", 8080);
    RAY_LOG_INF << "Server started";
    waitForTerminationRequest();
    svr->stop();
    return Application::EXIT_OK;
  }
};

std::atomic_bool EntryPoint::do_shutdown{false};

POCO_SERVER_MAIN(EntryPoint);
