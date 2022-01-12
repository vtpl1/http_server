// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include "pipeline_manager.h"
#include "pipeline.h"
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
    std::vector<Job> jobs = _jlm->get_jobs();
    for (auto &&job : jobs)
    {
      std::string command = "./build/entrypoint/Debug/media_converter.exe";
      std::vector<std::string> args;

      args.push_back("-i");
      args.push_back(job.input);
      args.push_back("-o");
      args.push_back(job.output);
      std::unique_ptr<Pipeline> rtmp_to_hls = std::make_unique<Pipeline>(command, args, "");
      _jlm->add_running_job(job);
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}