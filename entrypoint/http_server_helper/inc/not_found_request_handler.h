// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef not_found_request_handler_h
#define not_found_request_handler_h
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

class NotFoundRequestHandler : public Poco::Net::HTTPRequestHandler
{
private:
public:
  NotFoundRequestHandler() = default;
  ~NotFoundRequestHandler() = default;
  static void staticHandleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
  void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) override;
};

#endif // not_found_request_handler_h
