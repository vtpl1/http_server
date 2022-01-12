// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

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
  while (!_do_shutdown_composite()) {
    for (size_t i = 0; i < _jobs.size(); i++)
    {
      bool already_running = false;
      for (size_t j = 0; i < _running_jobs.size(); j++)
      {
        if (_jobs[i] == _running_jobs[j]) {
          already_running = true;
          break;
        }
      }
      if (already_running) {
        delete_job(i);
        i--;
      }

    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}
void JobListManager::add_job(Job job)
{
  std::lock_guard<std::mutex> lock(_jobs_mutex);
  _jobs.push_back(job);
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
void JobListManager::add_running_job(Job job)
{
  std::lock_guard<std::mutex> lock(_running_jobs_mutex);
  _running_jobs.push_back(job);
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