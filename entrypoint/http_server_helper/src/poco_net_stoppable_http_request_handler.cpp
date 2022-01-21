// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <Poco/Delegate.h>

#include "poco_net_stoppable_http_request_handler.h"

PocoNetStoppableHTTPRequestHandler::PocoNetStoppableHTTPRequestHandler(ServerStoppedEvent::Ptr server_stopped_event)
    : _server_stopped_event(std::move(server_stopped_event))
{
  _server_stopped_event->serverStopped += Poco::delegate(this, &PocoNetStoppableHTTPRequestHandler::onServerStopped);
}

PocoNetStoppableHTTPRequestHandler::~PocoNetStoppableHTTPRequestHandler()
{
  _server_stopped_event->serverStopped -= Poco::delegate(this, &PocoNetStoppableHTTPRequestHandler::onServerStopped);
}
void PocoNetStoppableHTTPRequestHandler::onServerStopped(const bool& abortCurrent)
{
  if (abortCurrent) {
    stopped = true;
  }
}