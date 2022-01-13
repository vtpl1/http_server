// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef job_list_manager_h
#define job_list_manager_h
#include <atomic>
#include <memory>
#include <thread>
#include <vector>
#include <mutex>

#include "job.h"

class JobListManager
{

private:
  std::atomic_bool _do_shutdown{false};
  std::atomic_bool _is_internal_shutdown{false};
  bool _is_already_shutting_down{false};
  inline bool _do_shutdown_composite() { return (_do_shutdown || _is_internal_shutdown); }

  std::unique_ptr<std::thread> _thread;
  std::mutex _jobs_mutex;
  std::mutex _running_jobs_mutex;
  std::vector<Job> _jobs;
  std::vector<Job> _running_jobs;

public:
  JobListManager() = default;
  ~JobListManager();
  void start();
  void signal_to_stop();
  void stop();
  void run();
  void add_job(Job job);
  void delete_job(size_t pos);
  std::vector<Job> get_jobs();
  void add_running_job(Job job);
  void delete_running_job(size_t pos);
  std::vector<Job> get_running_jobs();
  std::vector<Job> get_not_running_jobs();
};

#endif // job_list_manager_h
