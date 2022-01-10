// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef job_list_manager_h
#define job_list_manager_h
#include <atomic>
#include <memory>
#include <thread>

#include "job.h"

class JobListManager
{

private:
  std::atomic_bool _do_shutdown{false};
  std::atomic_bool _is_internal_shutdown{false};
  bool _is_already_shutting_down{false};
  inline bool _do_shutdown_composite() { return (_do_shutdown || _is_internal_shutdown); }

  std::unique_ptr<std::thread> _thread;

public:
  JobListManager() = default;
  ~JobListManager();
  void start();
  void signal_to_stop();
  void stop();
  void run();
};

#endif // job_list_manager_h
