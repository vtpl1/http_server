// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef command_receiver_h
#define command_receiver_h

#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <atomic>
#include <iostream>
#include <memory>
#include <thread>

class CommandReceiver
{
private:
  std::atomic_bool _do_shutdown{false};
  std::atomic_bool _is_internal_shutdown{false};
  bool _is_already_shutting_down{false};
  inline bool _do_shutdown_composite() { return (_do_shutdown || _is_internal_shutdown); }
  std::unique_ptr<std::thread> _thread;
  Poco::Net::HTTPClientSession cs;
  Poco::Net::HTTPRequest request;
  Poco::Net::HTTPResponse response;

public:
  CommandReceiver();
  ~CommandReceiver();
  void start();
  void signal_to_stop();
  void stop();
  void run();
};
#endif // command_receiver_h
