// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef http_server_h
#define http_server_h
#include <Poco/Net/HTTPServer.h>
#include <map>
#include <memory>
#include <string>

class HttpServer
{
private:
  int _port;
  std::unique_ptr<Poco::Net::HTTPServer> _srv;

  std::map<std::string, std::string> _base_dirs;
  std::map<std::string, std::string> _file_extension_and_mimetype_map;
  bool _is_already_shutting_down{false};
public:
  HttpServer(int port);
  ~HttpServer();
  void start();
  void signal_to_stop();
  void stop();
  bool set_mount_point(const std::string& mount_point, const std::string& dir);
  void set_file_extension_and_mimetype_mapping(const char* ext, const char* mime);
};

#endif // http_server_h
