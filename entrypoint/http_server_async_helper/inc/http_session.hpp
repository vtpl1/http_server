// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef http_session_hpp
#define http_session_hpp
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <functional>
#include <map>
#include <memory>
#include <string>

#include "http_server_data_models.h"

namespace http
{
namespace server
{
class HttpSession : public std::enable_shared_from_this<HttpSession>
{
public:
  HttpSession(const HttpSession&) = delete;
  HttpSession& operator=(const HttpSession&) = delete;

  explicit HttpSession(boost::asio::ip::tcp::socket socket, DocRoots& doc_roots);

  void run();

private:
  boost::asio::ip::tcp::socket socket_;

  boost::beast::flat_buffer buffer_;

  boost::beast::http::request<boost::beast::http::string_body> req_;

  DocRoots& doc_roots_;

  static void fail(boost::beast::error_code ec, char const* what);
  void do_read();
  void on_read(boost::beast::error_code ec, std::size_t bytes_transferred);
  void on_write(boost::beast::error_code ec, std::size_t bytes_transferred, bool close);
};
} // namespace server
} // namespace http

#endif // http_session_hpp
