// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <algorithm>

#include "job_to_media_command_mapper.h"
#include "pipeline_manager.h"

PipelineManager::PipelineManager(JobListManager& jlm, std::string base_dir) : _jlm(jlm), _base_dir(std::move(base_dir))
{
}

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
      std::vector<std::string> args;
      MediaCommand m = JobToMediaCommandMapper::get_media_command(job);
      args.emplace_back("-i");
      args.emplace_back(m.input);
      args.emplace_back("-o");
      args.emplace_back(m.output);
      _pipeline_map.insert(std::make_pair(job, std::make_unique<Pipeline>(m.command, args, _base_dir)));
      _jlm.add_running_job(job);
    }
    for (auto&& job : _jlm.get_extra_running_jobs()) {
      auto it = _pipeline_map.find(job);
      if (it != _pipeline_map.end()) {
        _pipeline_map.erase(it);
        _jlm.delete_running_job(job);
      }
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}