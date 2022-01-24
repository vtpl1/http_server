// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************
#include <Poco/File.h>
#include <Poco/Path.h>
#include <algorithm>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "http_server.h"
#include "logging.h"
// #include "server_certificate.h"

constexpr int EXPIRES_AFTER_TIME_OUT = 30;
constexpr int PARSER_BODY_LIMIT = 10000;

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
// namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl; // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

//------------------------------------------------------------------------------

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
  if (_ioc) {
    _ioc->stop();
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

// Return a reasonable mime type based on the extension of a file.
beast::string_view mime_type(beast::string_view path)
{
  using beast::iequals;
  auto const ext = [&path] {
    auto const pos = path.rfind(".");
    if (pos == beast::string_view::npos) {
      return beast::string_view{};
    }
    return path.substr(pos);
  }();
  if ((iequals(ext, ".htm")) || (iequals(ext, ".html")) || (iequals(ext, ".php"))) {
    return "text/html";
  }

  if (iequals(ext, ".css")) {
    return "text/css";
  }
  if (iequals(ext, ".txt")) {
    return "text/plain";
  }
  if (iequals(ext, ".js")) {
    return "application/javascript";
  }
  if (iequals(ext, ".json")) {
    return "application/json";
  }
  if (iequals(ext, ".xml")) {
    return "application/xml";
  }
  if (iequals(ext, ".swf")) {
    return "application/x-shockwave-flash";
  }
  if (iequals(ext, ".flv")) {
    return "video/x-flv";
  }
  if (iequals(ext, ".png")) {
    return "image/png";
  }
  if ((iequals(ext, ".jpe")) || (iequals(ext, ".jpeg")) || (iequals(ext, ".jpg"))) {
    return "image/jpeg";
  }
  if (iequals(ext, ".gif")) {
    return "image/gif";
  }
  if (iequals(ext, ".bmp")) {
    return "image/bmp";
  }
  // if (iequals(ext, ".ico")) {
  //   return "image/vnd.microsoft.icon";
  // }
  if (iequals(ext, ".ico")) {
    return "application/ico";
  }
  if ((iequals(ext, ".tiff")) || (iequals(ext, ".tif"))) {
    return "image/tiff";
  }
  if ((iequals(ext, ".svg")) || (iequals(ext, ".svgz"))) {
    return "image/svg+xml";
  }
  if (iequals(ext, ".ttf")) {
    return "application/octet-stream";
  }
  if (iequals(ext, ".woff")) {
    return "font/x-woff";
  }
  if (iequals(ext, ".woff2")) {
    return "font/x-woff2";
  }
  if (iequals(ext, ".ts")) {
    return "video/mp2t";
  }
  if ((iequals(ext, ".m3u8")) || (iequals(ext, ".tmp"))) {
    return "application/vnd.apple.mpegurl";
  }
  if (iequals(ext, ".pdf")) {
    return "application/pdf";
  }
  return "application/text";
}
// Report a failure
void fail(beast::error_code ec, char const* what) { RAY_LOG_ERR << what << ": " << ec.message(); }

// Handles an HTTP server connection
class http_session : public std::enable_shared_from_this<http_session>
{
  // This queue is used for HTTP pipelining.
  class queue
  {
    enum {
      // Maximum number of responses we will queue
      limit = 8
    };

    // The type-erased, saved work item
    struct work {
      virtual ~work() = default;
      virtual void operator()() = 0;
    };

    http_session& self_;
    std::vector<std::unique_ptr<work>> items_;

  public:
    explicit queue(http_session& self) : self_(self)
    {
      static_assert(limit > 0, "queue limit must be positive");
      items_.reserve(limit);
    }

    // Returns `true` if we have reached the queue limit
    bool is_full() const { return items_.size() >= limit; }

    // Called when a message finishes sending
    // Returns `true` if the caller should initiate a read
    bool on_write()
    {
      BOOST_ASSERT(!items_.empty());
      auto const was_full = is_full();
      items_.erase(items_.begin());
      if (!items_.empty()) {
        (*items_.front())();
      }
      return was_full;
    }

    // Called by the HTTP handler to send a response.
    template <bool isRequest, class Body, class Fields> void operator()(http::message<isRequest, Body, Fields>&& msg)
    {
      // This holds a work item
      struct work_impl : work {
        http_session& self_;
        http::message<isRequest, Body, Fields> msg_;

        work_impl(http_session& self, http::message<isRequest, Body, Fields>&& msg) : self_(self), msg_(std::move(msg))
        {
        }

        void operator()() override
        {
          http::async_write(
              self_.stream_, msg_,
              beast::bind_front_handler(&http_session::on_write, self_.shared_from_this(), msg_.need_eof()));
        }
      };

      // Allocate and store the work
      items_.push_back(boost::make_unique<work_impl>(self_, std::move(msg)));

      // If there was no previous work, start this one
      if (items_.size() == 1) {
        (*items_.front())();
      }
    }
  };

  beast::tcp_stream stream_;
  beast::flat_buffer buffer_;
  std::shared_ptr<DocRoots const> doc_roots_;
  queue queue_;

  // The parser is stored in an optional container so we can
  // construct it from scratch it at the beginning of each new message.
  boost::optional<http::request_parser<http::string_body>> parser_;

public:
  // Take ownership of the socket
  http_session(tcp::socket&& socket, std::shared_ptr<DocRoots const> doc_roots)
      : stream_(std::move(socket)), doc_roots_(std::move(doc_roots)), queue_(*this)
  {
  }
  // Start the session
  void run()
  {
    // We need to be executing within a strand to perform async operations
    // on the I/O objects in this session. Although not strictly necessary
    // for single-threaded contexts, this example code is written to be
    // thread-safe by default.
    net::dispatch(stream_.get_executor(), beast::bind_front_handler(&http_session::do_read, this->shared_from_this()));
  }

private:
  void do_read()
  {
    // Construct a new parser for each message
    parser_.emplace();

    // Apply a reasonable limit to the allowed size
    // of the body in bytes to prevent abuse.
    parser_->body_limit(PARSER_BODY_LIMIT);
    // Set the timeout.
    stream_.expires_after(std::chrono::seconds(EXPIRES_AFTER_TIME_OUT));
    // Read a request using the parser-oriented interface
    http::async_read(stream_, buffer_, *parser_, beast::bind_front_handler(&http_session::on_read, shared_from_this()));
  }
  void on_read(beast::error_code ec, std::size_t bytes_transferred)
  {
    boost::ignore_unused(bytes_transferred);

    // This means they closed the connection
    if (ec == http::error::end_of_stream) {
      return do_close();
    }

    if (ec) {
      return fail(ec, "read");
    }

    // See if it is a WebSocket Upgrade
    if (websocket::is_upgrade(parser_->get())) {
      // Create a websocket session, transferring ownership
      // of both the socket and the HTTP request.
      std::make_shared<websocket_session>(stream_.release_socket())->do_accept(parser_->release());
      return;
    }

    // Send the response
    handle_request(*doc_root_, parser_->release(), queue_);

    // If we aren't at the queue limit, try to pipeline another request
    if (!queue_.is_full()) {
      do_read();
    }
  }
  void on_write(bool close, beast::error_code ec, std::size_t bytes_transferred) {}
  void do_close() {}
};

// Accepts incoming connections and launches the sessions
class listener : public std::enable_shared_from_this<listener>
{
  net::io_context& ioc_;
  tcp::acceptor acceptor_;
  std::shared_ptr<DocRoots const> doc_roots_;

public:
  listener(net::io_context& ioc, const tcp::endpoint& endpoint, std::shared_ptr<DocRoots const> doc_roots)
      : ioc_(ioc), acceptor_(net::make_strand(ioc)), doc_roots_(std::move(doc_roots))
  {
    beast::error_code ec;

    // Open the acceptor
    acceptor_.open(endpoint.protocol(), ec);
    if (ec) {
      fail(ec, "open");
      return;
    }

    // Allow address reuse
    acceptor_.set_option(net::socket_base::reuse_address(true), ec);
    if (ec) {
      fail(ec, "set_option");
      return;
    }

    // Bind to the server address
    acceptor_.bind(endpoint, ec);
    if (ec) {
      fail(ec, "bind");
      return;
    }

    // Start listening for connections
    acceptor_.listen(net::socket_base::max_listen_connections, ec);
    if (ec) {
      fail(ec, "listen");
      return;
    }
  }
  // Start accepting incoming connections
  void run()
  {
    // We need to be executing within a strand to perform async operations
    // on the I/O objects in this session. Although not strictly necessary
    // for single-threaded contexts, this example code is written to be
    // thread-safe by default.
    net::dispatch(acceptor_.get_executor(), beast::bind_front_handler(&listener::do_accept, this->shared_from_this()));
  }

private:
  void do_accept()
  {
    // The new connection gets its own strand
    acceptor_.async_accept(net::make_strand(ioc_), beast::bind_front_handler(&listener::on_accept, shared_from_this()));
  }

  void on_accept(beast::error_code ec, tcp::socket socket)
  {
    if (ec) {
      fail(ec, "accept");
    } else {
      // Create the http session and run it
      std::make_shared<http_session>(std::move(socket), doc_roots_)->run();
    }

    // Accept another connection
    do_accept();
  }
};

void HttpServer::run()
{
  RAY_LOG_INF << "Started";
  auto const address = net::ip::make_address("0.0.0.0");
  auto const port = static_cast<uint16_t>(_port);
  auto const threads = 1;
  auto const doc_roots = std::make_shared<DocRoots const>(_base_dirs);
  // The io_context is required for all I/O
  _ioc = std::make_unique<net::io_context>(threads);
  // Create and launch a listening port
  std::make_shared<listener>(*_ioc, tcp::endpoint{address, port}, doc_roots)->run();
  _ioc->run();
  RAY_LOG_INF << "Stopped";
}