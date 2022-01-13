// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef file_request_handler_h
#define file_request_handler_h
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <string>

#include "poco_net_stoppable_http_request_handler.h"
#include "server_stopped_event.h"

class FileRequestHandler : public PocoNetStoppableHTTPRequestHandler
{
private:
  std::string _base_path;
  std::string _content_type;
  int _monitor_in_sec_for_availability;

public:
  FileRequestHandler(std::string base_path, std::string content_type, ServerStoppedEvent::Ptr server_stopped_event,
                     int monitor_in_sec_for_availability = 0);
  ~FileRequestHandler() = default;
  void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
};

#endif // file_request_handler_h
