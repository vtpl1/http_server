// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <Poco/Exception.h>
#include <Poco/Path.h>

#include "logging.h"
#include "pipeline.h"
Pipeline::Pipeline(std::string command) : Pipeline(std::move(command), std::vector<std::string>(), Poco::Path::current()) {}
Pipeline::Pipeline(std::string command, Poco::Process::Args args, std::string initial_directory)
    : _command(std::move(command)), _args(std::move(args)), _initial_directory(std::move(initial_directory))
{
}
Pipeline::~Pipeline() { stop(); }
void Pipeline::start()
{
  //_thread = std::make_unique<std::thread>(&Pipeline::run, this);
  _thread = std::make_unique<std::future<void>>(std::async(std::launch::async, &Pipeline::run, this));
}
void Pipeline::signal_to_stop()
{
  std::unique_lock<std::mutex> lock_thread_running(_thread_running_mutex);
  if (!_is_thread_running) {
    _thread_running_cv.wait(lock_thread_running);
  }

  _do_shutdown = true;
  if (_process_handle) {
    if (_process_handle->id() > 0) {
      std::cout << "signal_to_stop called" << std::endl;
      Poco::Process::requestTermination(_process_handle->id());
    }
  }
}
void Pipeline::stop()
{
  if (_is_already_shutting_down) {
    return;
  }
  _is_already_shutting_down = true;
  signal_to_stop();

  // if (_thread) {
  //   if (_thread->joinable()) {
  //     _thread->join();
  //   }
  // }
  if (_thread) {
    if (_thread->wait_for(std::chrono::seconds(2)) == std::future_status::timeout) {
      if (_process_handle) {
        Poco::Process::kill(*_process_handle);
      }
      _thread->wait();
    }
    _thread = nullptr;
  }
}
void Pipeline::run()
{
  std::stringstream ss;
  ss << "[";
  ss << _command;
  for (const auto& piece : _args) {
    ss << " ";
    ss << piece;
  }
  ss << "]";

  _composite_command = ss.str();

  RAY_LOG_INF << "Thread Started for " << _composite_command;
  while (!_do_shutdown_composite()) {
    RAY_LOG_INF << "Starting process " << _composite_command;
    {
      std::lock_guard<std::mutex> lock_thread_running(_thread_running_mutex);
      try {
        _process_handle = std::make_unique<Poco::ProcessHandle>(Poco::Process::launch(_command, _args, _initial_directory));
      } catch (Poco::Exception& e) {
        RAY_LOG_ERR << "MONOTOSH:: Poco::Exception " << e.what();
      } catch (const std::exception& e) {
        RAY_LOG_ERR << "MONOTOSH:: " << e.what();
      }
      _is_thread_running = true;
      _thread_running_cv.notify_all();
    }

    if (_process_handle) {
      if (_process_handle->id() > 0) {
        int exit_code = _process_handle->wait();
        RAY_LOG_INF << "Process returned with:: " << exit_code;
        _process_handle = nullptr;
      }
    } else {
      break;
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  // try {
  //   if (_process_handle) {
  //     if (_process_handle->id() > 0) {
  //       Poco::Process::kill(_process_handle->id());
  //     }
  //   }
  // } catch (Poco::Exception& exc) {
  //   RAY_LOG_ERR << "Killing process  failed" << _composite_command << " " << exc.displayText();
  // } catch (std::exception& exc) {
  //   RAY_LOG_ERR << "Killing process  failed" << _composite_command << " " << exc.what();
  // } catch (...) {
  //   RAY_LOG_ERR << "Killing process  failed" << _composite_command;
  // }
  RAY_LOG_INF << "Thread Stopped for " << _composite_command;
}

bool Pipeline::is_running()
{
  RAY_LOG_INF << "is running started";
  std::unique_lock<std::mutex> lock_thread_running(_thread_running_mutex);
  if (!_is_thread_running) {
    _thread_running_cv.wait(lock_thread_running);
  }
  RAY_LOG_INF << "is running returned";
  if (_process_handle) {
    if (_process_handle->id() > 0) {
      return true;
    }
  }

  return false;
}