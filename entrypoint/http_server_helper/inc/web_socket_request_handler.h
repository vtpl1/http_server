// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef web_socket_request_handler_h
#define web_socket_request_handler_h
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
class WebSocketRequestHandler : public Poco::Net::HTTPRequestHandler
{
public:
  WebSocketRequestHandler() = default;
  ~WebSocketRequestHandler() = default;
  void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) override;
};

#endif // web_socket_request_handler_h
