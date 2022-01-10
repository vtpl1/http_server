// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <Poco/Exception.h>

#include "logging.h"
#include "pipeline.h"

Pipeline::Pipeline(std::string command, Poco::Process::Args args, std::string initial_directory)
    : _command(std::move(command)), _args(std::move(args)), _initial_directory(std::move(initial_directory)), _pid(-1)
{
}
Pipeline::~Pipeline() { stop(); }
void Pipeline::start() { _thread = std::make_unique<std::thread>(&Pipeline::run, this); }
void Pipeline::signal_to_stop()
{
  _do_shutdown = true;
  if (_pid > 0) {
    Poco::Process::requestTermination(_pid);
  }
}
void Pipeline::stop()
{
  if (_is_already_shutting_down) {
    return;
  }
  _is_already_shutting_down = true;
  signal_to_stop();

  if (_thread) {
    if (_thread->joinable()) {
      _thread->join();
    }
  }
}
void Pipeline::run()
{
  std::stringstream ss;
  for (const auto& piece : _args) {
    ss << " ";
    ss << piece;
  }
  _composite_command = _command + " " + ss.str();

  RAY_LOG_INF << "Thread Started for " << _composite_command;
  while (!_do_shutdown_composite()) {
    RAY_LOG_INF << "Starting process " << _composite_command;
    Poco::ProcessHandle process_handle = Poco::Process::launch(_command, _args, _initial_directory);
    _pid = process_handle.id();
    if (_pid > 0) {
      int exit_code = process_handle.wait();
      _pid = 0;
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  try {
    if (_pid > 0) {
      Poco::Process::kill(_pid);
    }
  } catch (Poco::Exception& exc) {
    RAY_LOG_ERR << "Killing process  failed" << _composite_command << " " << exc.displayText();
  } catch (std::exception& exc) {
    RAY_LOG_ERR << "Killing process  failed" << _composite_command << " " << exc.what();
  } catch (...) {
    RAY_LOG_ERR << "Killing process  failed" << _composite_command;
  }
  RAY_LOG_INF << "Thread Stopped for " << _composite_command;
}