// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef method_not_supported_request_handler_cpp
#define method_not_supported_request_handler_cpp
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

class MethodNotSupportedRequestHandler : public Poco::Net::HTTPRequestHandler
{
private:
public:
  MethodNotSupportedRequestHandler() = default;
  ~MethodNotSupportedRequestHandler() = default;
  void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) override;
};

#endif // method_not_supported_request_handler_cpp