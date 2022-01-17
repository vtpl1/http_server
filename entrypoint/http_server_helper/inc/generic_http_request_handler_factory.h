// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef generic_http_request_handler_factory_h
#define generic_http_request_handler_factory_h
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/SharedPtr.h>
#include <functional>
#include <map>
#include <string>

#include "http_server_data_models.h"
#include "server_stopped_event.h"

class GenericHttpRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
private:
  std::map<std::string, std::string> _base_dirs;
  std::map<std::string, std::string> _file_extension_and_mimetype_map;
  std::map<std::string, int> _pattern_to_delay_map;
  std::map<std::string, URLCallBackHandler> _pattern_to_url_call_back_handler;
  std::vector<StatusCallBackHandler> _status_call_back_handler;
  std::vector<CommandCallBackHandler> _command_call_back_handler;
  ServerStoppedEvent::Ptr _server_stopped_event;

  static bool is_valid_path(const std::string& path);
  Poco::Net::HTTPRequestHandler* handle_file_request(const Poco::Net::HTTPServerRequest& req);

public:
  using Ptr = Poco::SharedPtr<GenericHttpRequestHandlerFactory>;
  Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request);
  GenericHttpRequestHandlerFactory(std::map<std::string, std::string> base_dirs,
                                   std::map<std::string, std::string> file_extension_and_mimetype_map,
                                   std::map<std::string, int> pattern_to_delay_map,
                                   std::map<std::string, URLCallBackHandler> pattern_to_url_call_back_handler,
                                   std::vector<StatusCallBackHandler> status_call_back_handler,
                                   std::vector<CommandCallBackHandler> command_call_back_handler);
  void signal_to_stop();
  ~GenericHttpRequestHandlerFactory() = default;
};

#endif // generic_http_request_handler_factory_h
