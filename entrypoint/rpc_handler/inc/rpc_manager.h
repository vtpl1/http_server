// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef rpc_manager_h
#define rpc_manager_h

#include <algorithm>
#include <atomic>
#include <cereal/archives/binary.hpp>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <sstream>
#include <thread>
#include <type_traits>
#include <vector>

#include "function_request_data.h"
#include "function_response_data.h"

using FunctionCallbackHandler = std::function<void(std::shared_ptr<std::vector<uint8_t>>)>;

template <typename R, typename... A> void is_void(R (*func)(A...)) { return std::is_void<R>::value; }

template <class T> std::shared_ptr<T> get_obj(std::shared_ptr<std::vector<uint8_t>> arg)
{
  std::shared_ptr<T> obj;
  if (arg != nullptr) {
    std::stringstream ss;

    std::copy(arg->begin(), arg->end(), std::ostream_iterator<uint8_t>(ss));

    {
      cereal::BinaryInputArchive iarchive(ss);
      obj = std::make_shared<T>();
      iarchive >> *obj;
    }
  }
  return obj;
}
// std::shared_ptr<std::vector<uint8_t>> put_obj()
// {
//   std::shared_ptr<std::vector<uint8_t>> ret;
//   return ret;
// }
template <typename T> std::shared_ptr<std::vector<uint8_t>> put_obj(const T& args)
{
  std::shared_ptr<std::vector<uint8_t>> ret;

  std::stringstream ss;
  {
    cereal::BinaryOutputArchive oarchive(ss);
    oarchive << CEREAL_NVP(args);
  }
  std::string s = ss.str();
  ret = std::make_shared<std::vector<uint8_t>>();
  std::copy(s.begin(), s.end(), std::back_inserter(*ret));

  return ret;
}

class RpcManager
{
private:
  std::atomic_bool _do_shutdown{false};
  std::atomic_bool _is_internal_shutdown{false};
  bool _is_already_shutting_down{false};
  inline bool _do_shutdown_composite() { return (_do_shutdown || _is_internal_shutdown); }

  std::unique_ptr<std::thread> _thread;
  std::map<std::string, FunctionCallbackHandler> _function_callback_map;
  std::queue<std::shared_ptr<std::vector<uint8_t>>> _send_q;
  std::queue<std::shared_ptr<std::vector<uint8_t>>> _receive_q;
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

  static void register_function(const std::string& func_name, const FunctionCallbackHandler& handler);
  static void call_remote_function(const std::string& func_name);
  static void call_remote_function(const std::string& func_name, std::shared_ptr<std::vector<uint8_t>> args);

  static std::shared_ptr<std::vector<uint8_t>> get_send_buffer();
  static void put_request_buffer(std::shared_ptr<std::vector<uint8_t>> buf);
};

#endif // rpc_manager_h
