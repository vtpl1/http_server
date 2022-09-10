// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************
#include <algorithm>
#include <cereal/archives/binary.hpp>
#include <logutil/logging.h>

#include "job_list_manager.h"
#include "rpc_manager.h"

JobListManager::JobListManager()
{
  RpcManager::register_function("add_job", [this](std::shared_ptr<std::vector<uint8_t>> arg) {
    add_job(*get_obj<Job>(std::move(arg)));
    RpcManager::call_remote_function("on_add_job_return");
  });

  RpcManager::register_function("on_add_job_return", [this](std::shared_ptr<std::vector<uint8_t>> /*ret*/) {});

  RpcManager::register_function("delete_job", [this](std::shared_ptr<std::vector<uint8_t>> arg) {
    delete_job(*get_obj<Job>(std::move(arg)));
    RpcManager::call_remote_function("on_delete_job_return");
  });

  RpcManager::register_function("on_delete_job_return", [this](std::shared_ptr<std::vector<uint8_t>> /*ret*/) {});

  RpcManager::register_function("update_job_list", [this](std::shared_ptr<std::vector<uint8_t>> arg) {
    update_job_list(*get_obj<JobList>(std::move(arg)));
    RpcManager::call_remote_function("on_update_job_list_return");
  });

  RpcManager::register_function("on_update_job_list_return", [this](std::shared_ptr<std::vector<uint8_t>> /*ret*/) {});

  RpcManager::register_function("clear_job_list", [this](std::shared_ptr<std::vector<uint8_t>> /*arg*/) {
    clear_job_list();
    RpcManager::call_remote_function("on_clear_job_list_return");
  });

  RpcManager::register_function("on_clear_job_list_return", [this](std::shared_ptr<std::vector<uint8_t>> /*ret*/) {});

  RpcManager::register_function("get_jobs", [this](std::shared_ptr<std::vector<uint8_t>> /*arg*/) {
    RpcManager::call_remote_function("on_get_jobs_return", put_obj<JobList>(get_jobs()));
  });

  RpcManager::register_function("on_get_jobs_return", [this](std::shared_ptr<std::vector<uint8_t>> ret) {
    // JobList job_list;
    update_job_list(*get_obj<JobList>(std::move(ret)));
  });
}
JobListManager& JobListManager::get_instance()
{
  static JobListManager instance;
  return instance;
}
JobListManager::~JobListManager() { stop(); }

void JobListManager::start(bool set_server_mode)
{
  get_instance()._is_server_mode = set_server_mode;
  get_instance().start_();
}
void JobListManager::start_()
{

  if (!_is_server_mode) {
    get_remote_jobs();
  }
  //_thread = std::make_unique<std::thread>(&JobListManager::run, this);
}
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

void JobListManager::update_job_list(const JobList& job_list)
{
  clear_job_list();
  RAY_LOG_INF << "updating job_list";
  for (auto&& j : job_list.job_list) {
    add_job(Job(j.channel_id));
  }
  if (_is_server_mode) {
    update_remote_job_list(job_list);
  }
}

void JobListManager::clear_job_list()
{
  std::lock_guard<std::mutex> lock(_jobs_mutex);
  _jobs.clear();
  if (_is_server_mode) {
    clear_remote_job_list();
  }
}

void JobListManager::add_job(const Job& job)
{
  RAY_LOG_INF << "adding job: " << job;
  std::lock_guard<std::mutex> lock(_jobs_mutex);
  if (std::find(_jobs.begin(), _jobs.end(), job) == _jobs.end()) {
    _jobs.emplace_back(job);
    if (_is_server_mode) {
      add_remote_job(job);
    }
  }
}
void JobListManager::delete_job(const Job& job)
{
  RAY_LOG_INF << "deleting job: " << job;
  std::lock_guard<std::mutex> lock(_jobs_mutex);
  auto it = std::find(_jobs.begin(), _jobs.end(), job);
  if (it != _jobs.end()) {
    if (_is_server_mode) {
      delete_remote_job(*it);
    }
    _jobs.erase(it);
  }
}
JobList JobListManager::get_jobs()
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
void JobListManager::delete_running_job(const Job& job)
{
  std::lock_guard<std::mutex> lock(_running_jobs_mutex);
  auto it = std::find(_running_jobs.begin(), _running_jobs.end(), job);
  if (it != _running_jobs.end()) {
    _running_jobs.erase(it);
  }
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
    bool is_match_found = false;
    for (auto&& running_job : _running_jobs) {
      if (running_job == job) {
        is_match_found = true;
      }
    }
    if (!is_match_found) {
      not_running_jobs.push_back(job);
    }
  }
  return not_running_jobs;
}

std::vector<Job> JobListManager::get_extra_running_jobs()
{
  std::lock_guard<std::mutex> running_lock(_running_jobs_mutex);
  std::lock_guard<std::mutex> lock(_jobs_mutex);
  std::vector<Job> extra_running_jobs;
  for (auto&& running_job : _running_jobs) {
    bool is_match_found = false;
    for (auto&& job : _jobs) {
      if (running_job == job) {
        is_match_found = true;
        break;
      }
    }
    if (!is_match_found) {
      extra_running_jobs.push_back(running_job);
    }
  }
  return extra_running_jobs;
}

void JobListManager::add_remote_job(const Job& job)
{
  // std::vector<uint8_t> args;
  // {
  //   std::stringstream ss;
  //   {
  //     cereal::BinaryOutputArchive oarchive(ss);
  //     oarchive << CEREAL_NVP(job);
  //   }
  //   std::string s = ss.str();
  //   std::copy(s.begin(), s.end(), std::back_inserter(args));
  // }
  RpcManager::call_remote_function("add_job", put_obj<Job>(job));
  // job
}
void JobListManager::update_remote_job_list(const JobList& job_list)
{
  // std::vector<uint8_t> args;
  // {
  //   std::stringstream ss;
  //   {
  //     cereal::BinaryOutputArchive oarchive(ss);
  //     oarchive << CEREAL_NVP(job_list);
  //   }
  //   std::string s = ss.str();
  //   std::copy(s.begin(), s.end(), std::back_inserter(args));
  // }
  RpcManager::call_remote_function("update_job_list", put_obj<JobList>(job_list));
  // job_list
}
void JobListManager::clear_remote_job_list()
{
  std::shared_ptr<std::vector<uint8_t>> args = std::make_shared<std::vector<uint8_t>>();
  RpcManager::call_remote_function("clear_job_list", args);
}
void JobListManager::delete_remote_job(const Job& job)
{
  // std::vector<uint8_t> args;
  // {
  //   std::stringstream ss;
  //   {
  //     cereal::BinaryOutputArchive oarchive(ss);
  //     oarchive << CEREAL_NVP(job);
  //   }
  //   std::string s = ss.str();
  //   std::copy(s.begin(), s.end(), std::back_inserter(args));
  // }
  RpcManager::call_remote_function("delete_job", put_obj<Job>(job));
}
void JobListManager::get_remote_jobs()
{
  std::shared_ptr<std::vector<uint8_t>> args = std::make_shared<std::vector<uint8_t>>();
  RpcManager::call_remote_function("get_jobs", args);
}
