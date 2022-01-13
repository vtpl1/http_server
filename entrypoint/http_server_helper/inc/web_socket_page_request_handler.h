// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef web_socket_page_request_handler_h
#define web_socket_page_request_handler_h
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
class WebSocketPageRequestHandler : public Poco::Net::HTTPRequestHandler
{
public:
    WebSocketPageRequestHandler() = default;
    ~WebSocketPageRequestHandler() = default;
    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) override;
};

#endif	// web_socket_page_request_handler_h
