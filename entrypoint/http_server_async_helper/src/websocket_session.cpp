// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include "websocket_session.hpp"

WebsocketSession::WebsocketSession(boost::asio::ip::tcp::socket socket, DocRoots& doc_roots)
    : socket_(std::move(socket)), doc_roots_(doc_roots)
{
}
void WebsocketSession::fail(boost::beast::error_code ec, char const* what)
{
  // Don't report on canceled operations
  if (ec == boost::beast::net::error::operation_aborted) {
    return;
  }

  std::cerr << what << ": " << ec.message() << "\n";
}
void WebsocketSession::do_read()
{
  // Read a message
  socket_.async_read(buffer_, [self = shared_from_this()](boost::beast::error_code ec, std::size_t bytes) {
    self->on_read(ec, bytes);
  });
}
void WebsocketSession::on_accept(boost::beast::error_code ec)
{
  // Handle the error, if any
  if (ec) {
    return fail(ec, "accept");
  }
  do_read();
}
void WebsocketSession::on_read(boost::beast::error_code ec, std::size_t bytes_transferred)
{
  boost::ignore_unused(bytes_transferred);

  // Handle the error, if any
  if (ec) {
    return fail(ec, "read");
  }

  //   // Send to all connections
  // state_->send(beast::buffers_to_string(buffer_.data()));
  // Clear the buffer
  buffer_.consume(buffer_.size());

  // Read another message
  do_read();
}
void WebsocketSession::on_write(boost::beast::error_code ec, std::size_t bytes_transferred)
{
  boost::ignore_unused(bytes_transferred);

  if (ec) {
    return fail(ec, "write");
  }
}