// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <Poco/Delegate.h>

#include "poco_net_stoppable_http_request_handler.h"

PocoNetStoppableHTTPRequestHandler::PocoNetStoppableHTTPRequestHandler(
    Poco::Net::HTTPRequestHandlerFactory::Ptr http_request_handler_factory)
    : _http_request_handler_factory(http_request_handler_factory)
{
  _http_request_handler_factory->serverStopped +=
      Poco::delegate(this, &PocoNetStoppableHTTPRequestHandler::onServerStopped);
}

PocoNetStoppableHTTPRequestHandler::~PocoNetStoppableHTTPRequestHandler()
{
  _http_request_handler_factory->serverStopped -=
      Poco::delegate(this, &PocoNetStoppableHTTPRequestHandler::onServerStopped);
}
void PocoNetStoppableHTTPRequestHandler::onServerStopped() { stopped = true; }