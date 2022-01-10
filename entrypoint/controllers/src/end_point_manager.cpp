// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include "end_point_manager.h"

EndPointManager::EndPointManager(JobListManager* jlm, int server_port) : _jlm(jlm), _server_port(server_port) {}
EndPointManager::~EndPointManager() { stop(); }
void EndPointManager::start() { _thread = std::make_unique<std::thread>(&EndPointManager::run, this); }
void EndPointManager::signal_to_stop()
{
  _do_shutdown = true;
  if (_svr) {
    _svr->stop();
  }
}
void EndPointManager::stop()
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
void EndPointManager::run()
{
  _svr = std::make_unique<httplib::Server>();
  _svr->listen("0.0.0.0", 8080);
}
