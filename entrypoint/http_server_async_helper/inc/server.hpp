// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef server_hpp
#define server_hpp
#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include "http_server_data_models.h"

namespace http
{
namespace server
{

class RequestHandler;
class ConnectionManager;
/// The top-level class of the HTTP server.
class Server
{
public:
  Server(const Server&) = delete;
  Server& operator=(const Server&) = delete;

  /// Construct the server to listen on the specified TCP address and port, and
  /// serve up files from the given directory.
  explicit Server(const std::string& address, uint16_t port, DocRoots& doc_roots);

  /// Run the server's io_context loop.
  void run();

  void signal_to_stop();

  void stop();

private:
  /// Perform an asynchronous accept operation.
  void do_accept();

  /// The io_context used to perform asynchronous operations.
  boost::asio::io_context io_context_;

  /// The signal_set is used to register for process termination notifications.
  // boost::asio::signal_set signals_;

  /// Acceptor used to listen for incoming connections.
  boost::asio::ip::tcp::acceptor acceptor_;

  DocRoots& doc_roots_;

  /// The connection manager which owns all live connections.
  //   ConnectionManager connection_manager_;

  /// The handler for all incoming requests.
  //   RequestHandler request_handler_;
  static void fail(boost::beast::error_code ec, char const* what);
};

} // namespace server

} // namespace http

#endif // server_hpp
