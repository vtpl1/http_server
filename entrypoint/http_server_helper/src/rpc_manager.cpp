// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include "rpc_manager.h"
#include "function_request_data.h"
#include "function_response_data.h"
#include "logging.h"
#include <cereal/archives/binary.hpp>

RpcManager::RpcManager() { start(); }

RpcManager::~RpcManager() { stop(); }

RpcManager& RpcManager::get_instance()
{
  static RpcManager instance;
  return instance;
}

void RpcManager::start() { _thread = std::make_unique<std::thread>(&RpcManager::run, this); }

void RpcManager::signal_to_stop() { _do_shutdown = true; }

void RpcManager::stop()
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

void RpcManager::run()
{
  RAY_LOG_INF << "Started..";
  while (!_do_shutdown_composite()) {
    if (_request_q.empty()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      continue;
    }

    FunctionRequestData data = _request_q.front();
    _request_q.pop();
    auto it = _function_callback_map.find(data.func_name);
    if (it != _function_callback_map.end()) {
      it->second(data.args);
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  RAY_LOG_INF << "Stopped..";
}

void RpcManager::register_callback_function()
{
  get_instance()._function_callback_map.emplace(
      std::make_pair("add_job", [](const std::vector<uint8_t>& serialized_job_data) {
        Job job;
        {
          int n = serialized_job_data.size();
          std::stringstream ss;
          std::copy(serialized_job_data.begin(), serialized_job_data.begin() + n, std::ostream_iterator<uint8_t>(ss));
          {
            cereal::BinaryInputArchive iarchive(ss);
            iarchive >> job;
          }
        }
        JobListManager::get_instance().add_job(job);
        return std::vector<uint8_t>();
      }));
  get_instance()._function_callback_map.emplace(
      std::make_pair("delete_job", [](const std::vector<uint8_t>& serialized_job_data) {
        Job job;
        {
          int n = serialized_job_data.size();
          std::stringstream ss;
          std::copy(serialized_job_data.begin(), serialized_job_data.begin() + n, std::ostream_iterator<uint8_t>(ss));
          {
            cereal::BinaryInputArchive iarchive(ss);
            iarchive >> job;
          }
        }
        JobListManager::get_instance().delete_job(job);
        return std::vector<uint8_t>();
      }));
  get_instance()._function_callback_map.emplace(
      std::make_pair("get_all_jobs", [](const std::vector<uint8_t>& data) {
        get_instance().response_to_get_all_jobs();
        return std::vector<uint8_t>();
      }));
}

void RpcManager::call_request(const std::string& func_name, const std::vector<uint8_t>& args)
{
  get_instance()._request_q.emplace(FunctionRequestData({func_name, args}));
}

void RpcManager::call_response(const std::string& func_name, const std::vector<uint8_t>& args)
{
  get_instance()._response_q.emplace(FunctionResponseData({func_name, args}));
}

void RpcManager::call_remote_request(const std::string& func_name, const std::vector<uint8_t>& args)
{
  get_instance()._remote_request_q.emplace(FunctionRequestData({func_name, args}));
}

void RpcManager::call_remote_response(const std::string& func_name, const std::vector<uint8_t>& args)
{
  get_instance()._remote_response_q.emplace(FunctionResponseData({func_name, args}));
}

std::unique_ptr<FunctionRequestData> RpcManager::get_remote_callable_request()
{
  std::unique_ptr<FunctionRequestData> data = nullptr;
  if (!get_instance()._remote_request_q.empty()) {
    data = std::make_unique<FunctionRequestData>(get_instance()._remote_request_q.front());
    get_instance()._remote_request_q.pop();
  }
  return data;
}

std::unique_ptr<FunctionResponseData> RpcManager::get_remote_callable_response()
{
  std::unique_ptr<FunctionResponseData> data = nullptr;
  if (!get_instance()._remote_response_q.empty()) {
    data = std::make_unique<FunctionResponseData>(get_instance()._remote_response_q.front());
    get_instance()._remote_response_q.pop();
  }
  return data;
}

void RpcManager::get_all_jobs()
{
  FunctionRequestData req;
  req.func_name = "get_all_jobs";
  {
    std::string s = "true";
    std::copy(s.begin(), s.end(), std::back_inserter(req.args));
  }
  RpcManager::get_instance().call_remote_request(req.func_name, req.args);
}

void RpcManager::response_to_get_all_jobs()
{
  for (auto&& job : JobListManager::get_instance().get_jobs()) {
    FunctionRequestData req;
    req.func_name = "add_job";
    {
      std::stringstream ss;
      {
        cereal::BinaryOutputArchive oarchive(ss);
        oarchive << CEREAL_NVP(job);
      }
      std::string s = ss.str();
      std::copy(s.begin(), s.end(), std::back_inserter(req.args));
    }
    RpcManager::get_instance().call_remote_request(req.func_name, req.args);
  }
}