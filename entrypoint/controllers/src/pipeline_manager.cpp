// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <algorithm>

#include "pipeline_manager.h"

PipelineManager::PipelineManager(JobListManager& jlm) : _jlm(jlm) {}

PipelineManager::~PipelineManager() { stop(); }
void PipelineManager::start() { _thread = std::make_unique<std::thread>(&PipelineManager::run, this); }
void PipelineManager::signal_to_stop() { _do_shutdown = true; }
void PipelineManager::stop()
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
void PipelineManager::run()
{
  while (!_do_shutdown_composite()) {
    for (auto&& job : _jlm.get_not_running_jobs()) {
      std::string command = "./build/entrypoint/Debug/media_converter.exe";
      std::vector<std::string> args;
      // FIXME: add media_command
      // args.emplace_back("-i");
      // args.emplace_back(job.input);
      // args.emplace_back("-o");
      // args.emplace_back(job.output);
      _pipeline_map.insert(std::make_pair(job, std::make_unique<Pipeline>(command, args, "")));
      _jlm.add_running_job(job);
    }
    for (auto&& job : _jlm.get_extra_running_jobs()) {
      auto it =_pipeline_map.find(job);
      if (it != _pipeline_map.end()) {
        _pipeline_map.erase(it);
        _jlm.delete_running_job(job);
      }
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}