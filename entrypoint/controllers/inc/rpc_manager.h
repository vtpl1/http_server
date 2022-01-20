// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef rpc_manager_h
#define rpc_manager_h

#include <atomic>
#include <map>
#include <memory>
#include <queue>
#include <thread>
#include <vector>

#include "function_request_data.h"
#include "function_response_data.h"

using FunctionCallbackHandler = std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)>;

class RpcManager
{
private:
  std::atomic_bool _do_shutdown{false};
  std::atomic_bool _is_internal_shutdown{false};
  bool _is_already_shutting_down{false};
  inline bool _do_shutdown_composite() { return (_do_shutdown || _is_internal_shutdown); }

  std::unique_ptr<std::thread> _thread;
  std::queue<FunctionRequestData> _request_q;
  std::queue<FunctionResponseData> _response_q;
  std::map<std::string, FunctionCallbackHandler> _function_callback_map;

  std::queue<FunctionRequestData> _remote_request_q;
  std::queue<FunctionResponseData> _remote_response_q;
  RpcManager();
  void start();
  void signal_to_stop();
  void stop();
  void run();
  // void _register_callback_function();
  // void _call_function(const std::string& func_name, const std::vector<uint8_t>& args);
  // void _call_remote_function(const std::string& func_name, const std::vector<uint8_t>& args);
  // std::unique_ptr<FunctionRequestData> _get_remote_callable_function();

public:
  ~RpcManager();
  static RpcManager& get_instance();
  static void register_callback_function();
  static void call_function(const std::string& func_name, const std::vector<uint8_t>& args);
  static void call_remote_function(const std::string& func_name, const std::vector<uint8_t>& args);
  std::unique_ptr<FunctionRequestData> get_remote_callable_function();

};

#endif // rpc_manager_h
