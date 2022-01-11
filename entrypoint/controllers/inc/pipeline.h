// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef pipeline_h
#define pipeline_h
#include <Poco/Process.h>
#include <atomic>
#include <memory>
#include <mutex>
#include <thread>
#include <future>

class Pipeline
{
private:
  std::atomic_bool _do_shutdown{false};
  std::atomic_bool _is_internal_shutdown{false};
  bool _is_already_shutting_down{false};
  inline bool _do_shutdown_composite() { return (_do_shutdown || _is_internal_shutdown); }

  // std::unique_ptr<std::thread> _thread;
  std::unique_ptr<std::future<void>> _thread;

  std::string _command;
  Poco::Process::Args _args;
  std::string _initial_directory;

  std::string _composite_command{};
  std::unique_ptr<Poco::ProcessHandle> _process_handle;
  std::mutex _thread_running_mutex;
  std::condition_variable _thread_running_cv;
  bool _is_thread_running{false};

public:
  Pipeline(std::string command);
  Pipeline(std::string command, Poco::Process::Args args, std::string initial_directory);
  ~Pipeline();
  void start();
  void signal_to_stop();
  void stop();
  void run();
  bool is_running();
};
#endif // pipeline_h
