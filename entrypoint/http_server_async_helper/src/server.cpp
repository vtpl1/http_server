// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <iostream>
#include <logutil/logging.h>

#include "http_session.hpp"
#include "server.hpp"

constexpr int CONCURRENCY_HINT = 1;
namespace http
{
namespace server
{
Server::Server(const std::string& address, uint16_t port, DocRoots& doc_roots)
    : io_context_(CONCURRENCY_HINT), acceptor_(boost::asio::make_strand(io_context_)), doc_roots_(doc_roots)
{
  // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
  boost::asio::ip::tcp::resolver resolver(io_context_);
  boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(address, std::to_string(port)).begin();
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();
}
void Server::signal_to_stop()
{
  boost::asio::post(io_context_, [this]() { acceptor_.close(); });
  io_context_.stop();
  RAY_LOG_INF << "IO context stop sent";
}
void Server::stop() { signal_to_stop(); }
void Server::run()
{
  do_accept();
  // The io_context::run() call will block until all asynchronous operations
  // have finished. While the server is running, there is always at least one
  // asynchronous operation outstanding: the asynchronous accept call waiting
  // for new incoming connections.
  io_context_.run();
}
void Server::fail(boost::beast::error_code ec, char const* what)
{
  // Don't report on canceled operations
  if (ec == boost::beast::net::error::operation_aborted) {
    return;
  }

  std::cerr << what << ": " << ec.message() << "\n";
}
void Server::do_accept()
{
  acceptor_.async_accept([this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
    // Check whether the server was stopped by a signal before this
    // completion handler had a chance to run.
    if (!acceptor_.is_open()) {
      return;
    }
    std::cout << "Connection received" << std::endl;

    if (ec) {
      return fail(ec, "accept");
    }

    std::make_shared<HttpSession>(std::move(socket), doc_roots_)->run();

    do_accept();
  });
}
} // namespace server
} // namespace http
