// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include "rpc_manager.h"
#include "function_request_data.h"
#include "function_response_data.h"
#include "logging.h"

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

void RpcManager::register_callback_function() {}
void RpcManager::call_function(const std::string& func_name, const std::vector<uint8_t>& args)
{
  get_instance()._request_q.emplace(FunctionRequestData({func_name, args}));
}
void RpcManager::call_remote_function(const std::string& func_name, const std::vector<uint8_t>& args)
{
  get_instance()._remote_request_q.emplace(FunctionRequestData({func_name, args}));
}

std::unique_ptr<FunctionRequestData> RpcManager::get_remote_callable_function()
{
  std::unique_ptr<FunctionRequestData> data;
  // if (!get_instance()._request_q.empty()) {
  //   data = std::make_unique<FunctionRequestData>(get_instance()._remote_request_q.front());
  //   get_instance()._remote_request_q.pop();
  // }
  return data;
}

std::unique_ptr<FunctionResponseData> RpcManager::get_remote_callable_response()
{
  std::unique_ptr<FunctionResponseData> data;
  return data;
}
void RpcManager::call_reponse(const std::string& func_name, const std::vector<uint8_t>& args)
{

}