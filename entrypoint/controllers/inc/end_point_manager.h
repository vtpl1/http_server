// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef end_point_manager_h
#define end_point_manager_h

#include <atomic>
#include <memory>
#include <thread>

#include "http_server.h"
#include "job_list_manager.h"

class EndPointManager
{
private:
  std::atomic_bool _do_shutdown{false};
  std::atomic_bool _is_internal_shutdown{false};
  bool _is_already_shutting_down{false};
  inline bool _do_shutdown_composite() { return (_do_shutdown || _is_internal_shutdown); }

  std::unique_ptr<std::thread> _thread;
  std::unique_ptr<HttpServer> _svr;

  JobListManager* _jlm;
  std::string _base_dir;
  int _server_port;

public:
  EndPointManager(JobListManager* jlm, std::string base_dir = "./", int server_port = 8080);
  ~EndPointManager();
  void start();
  void signal_to_stop();
  void stop();
  void run();
  void on_request_event(const std::string req_url);
};
#endif // end_point_manager_h
