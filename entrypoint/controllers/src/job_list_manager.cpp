// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include "job_list_manager.h"

JobListManager::JobListManager() {}

JobListManager::~JobListManager() { stop(); }
void JobListManager::start() { _thread.reset(new std::thread(&JobListManager::run, this)); }
void JobListManager::signal_to_stop() { _do_shutdown = true; }
void JobListManager::stop()
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
void JobListManager::run()
{
  while (!_do_shutdown_composite()) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}