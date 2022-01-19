// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef rpc_manager_h
#define rpc_manager_h

#include <atomic>
#include <memory>
#include <thread>
#include <queue>

#include "function_request_data.h"
#include "function_response_data.h"

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

public:
  RpcManager(/* args */);
  ~RpcManager();
  void start();
  void signal_to_stop();
  void stop();
  void run();
};

#endif // rpc_manager_h
