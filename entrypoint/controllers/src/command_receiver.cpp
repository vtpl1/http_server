// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include "command_receiver.h"

CommandReceiver::CommandReceiver(/* args */) {}

CommandReceiver::~CommandReceiver() { stop(); }

void CommandReceiver::start() { _thread = std::make_unique<std::thread>(&CommandReceiver::run, this); }
void CommandReceiver::signal_to_stop() { _do_shutdown = true; }
void CommandReceiver::stop()
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

void CommandReceiver::run()
{

}