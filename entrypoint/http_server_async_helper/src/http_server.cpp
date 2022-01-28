// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <Poco/File.h>
#include <Poco/Path.h>

#include "http_server.h"


constexpr int EXPIRES_AFTER_TIME_OUT = 30;
constexpr int PARSER_BODY_LIMIT = 10000;

HttpServer::HttpServer(int port) : _port(port) {}
void HttpServer::signal_to_stop()
{
  if (_is_already_shutting_down) {
    return;
  }
  _is_already_shutting_down = true;
  // Stop the `io_context`. This will cause `run()`
  // to return immediately, eventually destroying the
  // `io_context` and all of the sockets in it.
  if (_server) {
    _server->stop();
  }
}
void HttpServer::stop()
{
  signal_to_stop();
  if (_thread) {
    if (_thread->joinable()) {
      _thread->join();
    }
  }
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
void HttpServer::set_url_call_back_handler(const std::string& pattern, URLCallBackHandler handler)
{
  _pattern_to_url_call_back_handler[pattern] = std::move(handler);
}
void HttpServer::set_file_extension_and_mimetype_mapping(const char* ext, const char* mime)
{
  _file_extension_and_mimetype_map[ext] = mime;
}
void HttpServer::set_status_call_back_handler(StatusCallBackHandler handler)
{
  _status_call_back_handler.emplace_back(std::move(handler));
}
void HttpServer::set_command_call_back_handler(CommandCallBackHandler handler)
{
  _command_call_back_handler.emplace_back(std::move(handler));
}
void HttpServer::start() { _thread = std::make_unique<std::thread>(&HttpServer::run, this); }
void HttpServer::run()
{
  RAY_LOG_INF << "Started";
  // auto const address = boost::asio::ip::make_address("0.0.0.0");
  auto const port = static_cast<uint16_t>(_port);
  // auto const threads = 1;
  // auto const doc_roots = std::make_shared<DocRoots const>(_base_dirs);
  _server = std::make_unique<http::server::Server>("0.0.0.0", port, _base_dirs);
  // The io_context is required for all I/O
  // _ioc = std::make_unique<net::io_context>(threads);
  // // Create and launch a listening port
  // std::make_shared<Listener>(*_ioc, tcp::endpoint{address, port}, doc_roots)->run();
  // _ioc->run();
  _server->run();

  RAY_LOG_INF << "Stopped";
}
