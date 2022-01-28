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
#include <mutex>

#include "function_request_data.h"
#include "function_response_data.h"

using FunctionCallbackHandler = std::function<void(const std::vector<uint8_t>&)>;

class RpcManager
{
private:
  std::atomic_bool _do_shutdown{false};
  std::atomic_bool _is_internal_shutdown{false};
  bool _is_already_shutting_down{false};
  inline bool _do_shutdown_composite() { return (_do_shutdown || _is_internal_shutdown); }

  std::unique_ptr<std::thread> _thread;
  std::map<std::string, FunctionCallbackHandler> _function_callback_map;
  std::queue<std::vector<uint8_t>> _send_q;
  std::queue<std::vector<uint8_t>> _receive_q;
  std::mutex _send_q_mutex;
  std::mutex _receive_q_mutex;
  RpcManager();
  void start();
  void signal_to_stop();
  void stop();
  void run();
  static RpcManager& get_instance();

public:
  ~RpcManager();

  static void register_function(const std::string& func_name, FunctionCallbackHandler);
  static void call_remote_function(const std::string& func_name, const std::vector<uint8_t>& args);

  static std::vector<uint8_t> get_send_buffer();
  static void put_request_buffer(const std::vector<uint8_t>& buf);

};

#endif // rpc_manager_h
