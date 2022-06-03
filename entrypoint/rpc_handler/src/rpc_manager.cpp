// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <cereal/archives/binary.hpp>
#include <logutil/logging.h>

#include "function_request_data.h"
#include "function_response_data.h"
#include "rpc_manager.h"

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
    if (_receive_q.empty()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      continue;
    }
    std::shared_ptr<std::vector<uint8_t>> data = _receive_q.front();
    _receive_q.pop();
    std::shared_ptr<FunctionRequestData> req = get_obj<FunctionRequestData>(data);
    auto it = _function_callback_map.find(req->func_name);
    if (it != _function_callback_map.end()) {
      std::cout << "calling: " << req->func_name << std::endl;
      it->second(std::make_shared<std::vector<uint8_t>>(req->args));
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  RAY_LOG_INF << "Stopped..";
}

void RpcManager::register_function(const std::string& func_name, const FunctionCallbackHandler& handler)
{
  get_instance()._function_callback_map.emplace(std::make_pair(func_name, handler));
}
void RpcManager::call_remote_function(const std::string& func_name)
{
  call_remote_function(func_name, std::shared_ptr<std::vector<uint8_t>>());
}
void RpcManager::call_remote_function(const std::string& func_name, std::shared_ptr<std::vector<uint8_t>> args)
{
  std::shared_ptr<std::vector<uint8_t>> buffer;
  if (args == nullptr) {
    buffer = put_obj<FunctionRequestData>(FunctionRequestData(func_name, std::vector<uint8_t>()));
  } else {
    buffer = put_obj<FunctionRequestData>(FunctionRequestData(func_name, *std::move(args)));
  }
  std::lock_guard<std::mutex> lock(get_instance()._send_q_mutex);
  get_instance()._send_q.emplace(buffer);
}

std::shared_ptr<std::vector<uint8_t>> RpcManager::get_send_buffer()
{
  std::shared_ptr<std::vector<uint8_t>> buffer;
  std::lock_guard<std::mutex> lock(get_instance()._send_q_mutex);
  if (!get_instance()._send_q.empty()) {
    buffer = get_instance()._send_q.front();
    get_instance()._send_q.pop();
  }
  return buffer;
}

void RpcManager::put_request_buffer(std::shared_ptr<std::vector<uint8_t>> buf)
{
  std::lock_guard<std::mutex> lock(get_instance()._receive_q_mutex);
  get_instance()._receive_q.emplace(std::move(buf));
}
