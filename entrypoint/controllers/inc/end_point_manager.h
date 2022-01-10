// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef end_point_manager_h
#define end_point_manager_h

#include <atomic>
#include <httplib.h>
#include <memory>
#include <thread>

#include "job_list_manager.h"

class EndPointManager
{
private:
  std::atomic_bool _do_shutdown{false};
  std::atomic_bool _is_internal_shutdown{false};
  bool _is_already_shutting_down{false};
  inline bool _do_shutdown_composite() { return (_do_shutdown || _is_internal_shutdown); }

  std::unique_ptr<std::thread> _thread;
  std::unique_ptr<httplib::Server> _svr;

  JobListManager* _jlm;
  int _server_port;

public:
  EndPointManager(JobListManager* jlm, int server_port = 8080);
  ~EndPointManager();
  void start();
  void signal_to_stop();
  void stop();
  void run();
};
#endif // end_point_manager_h
