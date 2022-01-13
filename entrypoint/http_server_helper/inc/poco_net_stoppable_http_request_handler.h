// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef poco_net_stoppable_http_request_handler_h
#define poco_net_stoppable_http_request_handler_h
#include <Poco/Net/HTTPRequestHandler.h>

#include "server_stopped_event.h"

class PocoNetStoppableHTTPRequestHandler : public Poco::Net::HTTPRequestHandler
{
private:
protected:
  bool stopped{false};
  ServerStoppedEvent::Ptr _server_stopped_event;

public:
  PocoNetStoppableHTTPRequestHandler(ServerStoppedEvent::Ptr server_stopped_event);
  virtual ~PocoNetStoppableHTTPRequestHandler();
  void onServerStopped(const bool& abortCurrent);
};

#endif // poco_net_stoppable_http_request_handler_h
