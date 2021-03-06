// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef websocket_session_hpp
#define websocket_session_hpp
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <functional>
#include <map>
#include <memory>
#include <string>

#include "http_server_data_models.h"

class WebsocketSession : public std::enable_shared_from_this<WebsocketSession>
{
public:
  WebsocketSession(const WebsocketSession&) = delete;
  WebsocketSession& operator=(const WebsocketSession&) = delete;

  explicit WebsocketSession(boost::asio::ip::tcp::socket socket, DocRoots& doc_roots);

  template <class Body, class Allocator>
  void run(boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>>&& req)
  {
    // Accept the websocket handshake
    socket_.async_accept(req, [self = shared_from_this()](boost::beast::error_code ec) { self->on_accept(ec); });
  }

private:
  boost::beast::websocket::stream<boost::asio::ip::tcp::socket> socket_;

  boost::beast::flat_buffer buffer_;

  boost::beast::http::request<boost::beast::http::string_body> req_;

  DocRoots& doc_roots_;

  static void fail(boost::beast::error_code ec, char const* what);
  void do_read();
  void on_accept(boost::beast::error_code ec);
  void on_read(boost::beast::error_code ec, std::size_t bytes_transferred);
  void on_write(boost::beast::error_code ec, std::size_t bytes_transferred);
};

#endif // websocket_session_hpp
