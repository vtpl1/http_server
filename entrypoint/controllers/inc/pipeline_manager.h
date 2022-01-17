// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef pipeline_manager_h
#define pipeline_manager_h
#include <atomic>
#include <memory>
#include <thread>
// #include <unordered_map>
#include <map>

#include "job.h"
#include "job_list_manager.h"
#include "pipeline.h"

class PipelineManager
{
private:
  std::atomic_bool _do_shutdown{false};
  std::atomic_bool _is_internal_shutdown{false};
  bool _is_already_shutting_down{false};
  inline bool _do_shutdown_composite() { return (_do_shutdown || _is_internal_shutdown); }

  std::unique_ptr<std::thread> _thread;
  JobListManager& _jlm;
  std::map<Job, std::unique_ptr<Pipeline>> _pipeline_map;

public:
  PipelineManager(JobListManager& jlm);
  ~PipelineManager();
  void start();
  void signal_to_stop();
  void stop();
  void run();
};

#endif // pipeline_manager_h
