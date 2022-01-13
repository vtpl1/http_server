// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************
#include <algorithm>

#include "job_list_manager.h"

JobListManager::~JobListManager() { stop(); }
void JobListManager::start() { _thread = std::make_unique<std::thread>(&JobListManager::run, this); }
void JobListManager::signal_to_stop() { _do_shutdown = true; }
void JobListManager::stop()
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
void JobListManager::run()
{
  // while (!_do_shutdown_composite()) {

  //   std::this_thread::sleep_for(std::chrono::seconds(1));
  // }
}
void JobListManager::add_job(const Job& job)
{
  std::lock_guard<std::mutex> lock(_jobs_mutex);
  if (std::find(_jobs.begin(), _jobs.end(), job) == _jobs.end()) {
    _jobs.emplace_back(job);
  }
}
void JobListManager::delete_job(size_t pos)
{
  std::lock_guard<std::mutex> lock(_jobs_mutex);
  _jobs.erase(_jobs.begin() + pos);
}
std::vector<Job> JobListManager::get_jobs()
{
  std::lock_guard<std::mutex> lock(_jobs_mutex);
  return _jobs;
}
void JobListManager::add_running_job(const Job& job)
{
  std::lock_guard<std::mutex> lock(_running_jobs_mutex);
  if (std::find(_running_jobs.begin(), _running_jobs.end(), job) == _running_jobs.end()) {
    _running_jobs.emplace_back(job);
  }
}
void JobListManager::delete_running_job(size_t pos)
{
  std::lock_guard<std::mutex> lock(_running_jobs_mutex);
  _running_jobs.erase(_running_jobs.begin() + pos);
}
std::vector<Job> JobListManager::get_running_jobs()
{
  std::lock_guard<std::mutex> lock(_running_jobs_mutex);
  return _running_jobs;
}
std::vector<Job> JobListManager::get_not_running_jobs()
{
  std::lock_guard<std::mutex> running_lock(_running_jobs_mutex);
  std::lock_guard<std::mutex> lock(_jobs_mutex);
  std::vector<Job> not_running_jobs;
  for (auto&& job : _jobs) {
    bool is_already_running = false;
    for (auto&& running_job : _running_jobs) {
      if (running_job == job) {
        is_already_running = true;
      }
    }
    if (!is_already_running) {
      not_running_jobs.push_back(job);
    }
  }
  return not_running_jobs;
}
