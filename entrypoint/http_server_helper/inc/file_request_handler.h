// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef file_request_handler_h
#define file_request_handler_h
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <string>

class FileRequestHandler : public Poco::Net::HTTPRequestHandler
{
private:
  	std::string _base_path;
	std::string _content_type;

public:
  FileRequestHandler(std::string base_path, std::string content_type);
  ~FileRequestHandler() = default;
  void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
};

#endif // file_request_handler_h
