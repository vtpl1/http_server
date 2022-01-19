// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include "rpc_manager.h"
#include "function_request_data.h"
#include "function_response_data.h"

RpcManager::RpcManager() {}

RpcManager::~RpcManager() { stop(); }

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
//   while (!_do_shutdown_composite()) {
//   }
}