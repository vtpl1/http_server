// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef web_socket_request_handler_h
#define web_socket_request_handler_h
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/WebSocket.h>

#include <vector>

#include "http_server_data_models.h"
#include "poco_net_stoppable_http_request_handler.h"
#include "server_stopped_event.h"

class WebSocketRequestHandler : public PocoNetStoppableHTTPRequestHandler
{
public:
  WebSocketRequestHandler(ServerStoppedEvent::Ptr server_stopped_event);
  ~WebSocketRequestHandler() = default;
  void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) override;
};

#endif // web_socket_request_handler_h
