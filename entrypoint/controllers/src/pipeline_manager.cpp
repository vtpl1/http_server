// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include "pipeline_manager.h"
PipelineManager::PipelineManager(JobListManager* jlm) : _jlm(jlm) {}

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
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}