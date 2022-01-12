// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef generic_http_request_handler_factory_h
#define generic_http_request_handler_factory_h
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <map>
#include <string>

class GenericHttpRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
private:
  std::map<std::string, std::string> _base_dirs;
  std::map<std::string, std::string> _file_extension_and_mimetype_map;
  bool is_valid_path(const std::string &path);
  Poco::Net::HTTPRequestHandler* handle_file_request(const Poco::Net::HTTPServerRequest& req);

public:
  Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request);

  GenericHttpRequestHandlerFactory(std::map<std::string, std::string> base_dirs, std::map<std::string, std::string> file_extension_and_mimetype_map);
  ~GenericHttpRequestHandlerFactory();
};

#endif // generic_http_request_handler_factory_h
