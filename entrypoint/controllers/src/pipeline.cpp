// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include "pipeline.h"
#include "logging.h"
Pipeline::Pipeline() {}

Pipeline::~Pipeline() { stop(); }
void Pipeline::start() { _thread.reset(new std::thread(&Pipeline::run, this)); }
void Pipeline::signal_to_stop() { _do_shutdown = true; }
void Pipeline::stop()
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
void Pipeline::run()
{
  while (!_do_shutdown_composite()) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}