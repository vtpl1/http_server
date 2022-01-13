// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <Poco/File.h>
#include <Poco/Path.h>

#include "http_server.h"
// #include "logging.h"

constexpr int MAX_REQUEST_QUEUE_SIZE = 100;
constexpr int MAX_THREADS = 16;

HttpServer::HttpServer(int port) : _port(port)
{
  _file_extension_and_mimetype_map["htm"] = "text/html";
  _file_extension_and_mimetype_map["html"] = "text/html";
  _file_extension_and_mimetype_map["ico"] = "application/ico";
  _file_extension_and_mimetype_map["jpg"] = "image/jpeg";
  _file_extension_and_mimetype_map["jpeg"] = "image/jpeg";
  _file_extension_and_mimetype_map["map"] = "text/plain";
  _file_extension_and_mimetype_map["json"] = "application/json";
  _file_extension_and_mimetype_map["js"] = "application/javascript";
  _file_extension_and_mimetype_map["css"] = "text/css";
  _file_extension_and_mimetype_map["ttf"] = "application/octet-stream";
  _file_extension_and_mimetype_map["woff"] = "font/x-woff";
  _file_extension_and_mimetype_map["woff2"] = "font/x-woff2";
  _file_extension_and_mimetype_map["ts"] = "video/mp2t";
  _file_extension_and_mimetype_map["m3u8"] = "application/vnd.apple.mpegurl";
  _file_extension_and_mimetype_map["m3u8.tmp"] = "application/vnd.apple.mpegurl";
  _file_extension_and_mimetype_map["pdf"] = "application/pdf";
}
void HttpServer::start()
{
  Poco::Net::HTTPServerParams::Ptr http_server_params = new Poco::Net::HTTPServerParams();
  http_server_params->setMaxQueued(MAX_REQUEST_QUEUE_SIZE);
  http_server_params->setMaxThreads(MAX_THREADS);
  Poco::Net::ServerSocket svs(_port);
  svs.setReuseAddress(true);
  svs.setReusePort(false);
  _generic_http_request_handler_factory = new GenericHttpRequestHandlerFactory(
      _base_dirs, _file_extension_and_mimetype_map, _pattern_to_delay_map, _pattern_to_callback_map);
  _srv = std::make_unique<Poco::Net::HTTPServer>(_generic_http_request_handler_factory, svs, http_server_params);
  _srv->start();
}
void HttpServer::signal_to_stop()
{
  if (_is_already_shutting_down) {
    return;
  }
  _is_already_shutting_down = true;
  if (_generic_http_request_handler_factory) {
    _generic_http_request_handler_factory->signal_to_stop();
  }
  if (_srv) {
    _srv->stopAll(true);
  }
}
void HttpServer::stop()
{
  signal_to_stop();
  _srv = nullptr;
}
HttpServer::~HttpServer() { stop(); }
bool HttpServer::set_mount_point(const std::string& mount_point, const std::string& dir)
{
  try {
    Poco::File f(dir);
    if (f.isDirectory()) {
      std::string mnt = !mount_point.empty() ? mount_point : "/";
      if (!mnt.empty() && mnt[0] == '/') {
        _base_dirs.emplace(mnt, dir);
        return true;
      }
    }
  } catch (const std::exception& e) {
    // RAY_LOG_ERR << " " << dir << " " << e.what();
  }
  return false;
}
void HttpServer::set_delay_for_mount_point(const std::string& pattern, const int delay_in_sec)
{
  _pattern_to_delay_map[pattern] = delay_in_sec;
}
void HttpServer::set_callback_handler(const std::string& pattern, std::function<void(std::string)> handler)
{
  _pattern_to_callback_map[pattern] = handler;
}
void HttpServer::set_file_extension_and_mimetype_mapping(const char* ext, const char* mime)
{
  _file_extension_and_mimetype_map[ext] = mime;
}
