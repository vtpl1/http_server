// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef poco_net_stoppable_http_request_handler_h
#define poco_net_stoppable_http_request_handler_h
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>

class PocoNetStoppableHTTPRequestHandler : public Poco::Net::HTTPRequestHandler
{
private:
protected:
  bool stopped{false};
    Poco::Net::HTTPRequestHandlerFactory::Ptr _http_request_handler_factory;
public:
  PocoNetStoppableHTTPRequestHandler(Poco::Net::HTTPRequestHandlerFactory::Ptr http_request_handler_factory);
  ~PocoNetStoppableHTTPRequestHandler();
  void onServerStopped();
};

#endif // poco_net_stoppable_http_request_handler_h
