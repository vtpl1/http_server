// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef http_server_h
#define http_server_h
#include "pch.h"

#include <functional>
#include <map>
#include <memory>
#include <string>

#include "http_server_data_models.h"

namespace net = boost::asio; // from <boost/asio.hpp>

using DocRoots = std::map<std::string const, std::string const>;

class HttpServer
{
private:
  int _port;
  bool _is_already_shutting_down{false};
  DocRoots _base_dirs;
  std::map<std::string, std::string> _file_extension_and_mimetype_map;
  std::map<std::string, int> _pattern_to_delay_map;
  std::map<std::string, URLCallBackHandler> _pattern_to_url_call_back_handler;
  std::vector<StatusCallBackHandler> _status_call_back_handler;
  std::vector<CommandCallBackHandler> _command_call_back_handler;
  std::unique_ptr<std::thread> _thread;
  std::unique_ptr<net::io_context> _ioc;

  void run();

public:
  HttpServer(int port);
  ~HttpServer();
  void start();
  void signal_to_stop();
  void stop();
  bool set_mount_point(const std::string& mount_point, const std::string& dir);
  void set_delay_for_mount_point(const std::string& pattern, const int delay_in_sec);
  void set_url_call_back_handler(const std::string& pattern, URLCallBackHandler handler);
  void set_status_call_back_handler(StatusCallBackHandler handler);
  void set_command_call_back_handler(CommandCallBackHandler handler);
  void set_file_extension_and_mimetype_mapping(const char* ext, const char* mime);
};

#endif // http_server_h
