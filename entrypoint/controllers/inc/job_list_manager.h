// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef job_list_manager_h
#define job_list_manager_h
#include <atomic>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

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
  JobListManager() = default;

public:
  JobListManager(const JobListManager&) = delete;
  JobListManager& operator=(const JobListManager&) = delete;
  ~JobListManager();
  static JobListManager& get_instance();
  void start();
  void signal_to_stop();
  void stop();
  void run();
  void add_job(const Job& job);
  void update_job_list(const JobList& job_list);
  void clear_job_list();
  void delete_job(const Job& job);
  std::vector<Job> get_jobs();
  void add_running_job(const Job& job);
  void delete_running_job(const Job& job);
  std::vector<Job> get_running_jobs();
  std::vector<Job> get_not_running_jobs();
  std::vector<Job> get_extra_running_jobs();
};

#endif // job_list_manager_h
