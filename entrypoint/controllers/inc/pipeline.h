// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef pipeline_h
#define pipeline_h
#include <atomic>
#include <thread>
#include <memory>

class Pipeline
{
private:
  std::atomic_bool _do_shutdown{false};
  std::atomic_bool _is_internal_shutdown{false};
  bool _is_already_shutting_down{false};
  inline bool _do_shutdown_composite() { return (_do_shutdown || _is_internal_shutdown); }

  std::unique_ptr<std::thread> _thread;
public:
    Pipeline();
    ~Pipeline();
    void start();
    void signal_to_stop();
    void stop();
    void run();
};
#endif	// pipeline_h
