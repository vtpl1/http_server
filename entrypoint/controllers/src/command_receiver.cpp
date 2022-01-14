// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************
#include <Poco/Exception.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPMessage.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/NetException.h>
#include <Poco/Net/WebSocket.h>
#include <array>
#include <string>

#include "command_receiver.h"
#include "logging.h"

constexpr int MAX_BUFFER_SIZE = 1024;
constexpr int RECEIVE_TIMEOUT_MILLISEC = 500;
constexpr int PING_SEND_INTERVAL_SEC = 10;

CommandReceiver::CommandReceiver(std::string host, int port, JobListManager& jlm) : _host(host), _port(port), _jlm(jlm) {}

CommandReceiver::~CommandReceiver() { stop(); }

void CommandReceiver::start() { _thread = std::make_unique<std::thread>(&CommandReceiver::run, this); }
void CommandReceiver::signal_to_stop() { _do_shutdown = true; }
void CommandReceiver::stop()
{
  if (_is_already_shutting_down) {
    return;
  }
  _is_already_shutting_down = true;
  signal_to_stop();

  if (_thread) {
    if (_thread->joinable()) {
      _thread->join();
    }
  }
}

void CommandReceiver::run()
{
  auto last_op_time = std::chrono::high_resolution_clock::now();
  while (!_do_shutdown_composite()) {
    try {
      Poco::Net::HTTPClientSession cs("localhost", 8080);
      Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, "/ws", Poco::Net::HTTPMessage::HTTP_1_1);
      Poco::Net::HTTPResponse response;
      Poco::Net::WebSocket ws(cs, request, response);
      ws.setReceiveTimeout(
          Poco::Timespan(0, RECEIVE_TIMEOUT_MILLISEC * 1000)); // Timespan(long seconds, long microseconds)

      RAY_LOG_INF << "WebSocket connection established.";
      std::array<char, MAX_BUFFER_SIZE> buffer{};
      int flags = 0;
      int n = 0;
      while (!_do_shutdown_composite()) {
        bool is_timeout = false;
        flags = 0;
        n = 0;
        try {
          n = ws.receiveFrame(buffer.data(), sizeof(buffer), flags);
          RAY_LOG_INF << Poco::format("Frame received (length=%d, flags=0x%x).", n, unsigned(flags));
        } catch (Poco::TimeoutException& e) {
          is_timeout = true;
        } catch (Poco::Net::NetException& e) {
          RAY_LOG_INF << e.what();
          break;
        }
        if (is_timeout) {
          if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - last_op_time)
                  .count() >= PING_SEND_INTERVAL_SEC) {
            try {
              ws.sendFrame(buffer.data(), 0, Poco::Net::WebSocket::FRAME_OP_PING);
              last_op_time = std::chrono::high_resolution_clock::now();
              RAY_LOG_INF << "PING sent";
            } catch (Poco::Net::NetException& e) {
              RAY_LOG_ERR << e.what();
              break;
            }
          }
        }
        if ((flags & Poco::Net::WebSocket::FRAME_OP_BITMASK) == Poco::Net::WebSocket::FRAME_OP_CLOSE) {
          break;
        }
      }
      try {
        ws.close();
      } catch (Poco::Net::NetException& e) {
        RAY_LOG_INF << "Exception " << e.what();
      }
      RAY_LOG_INF << "WebSocket connection closed.";

    } catch (Poco::Net::WebSocketException& exc) {
      RAY_LOG_ERR << exc.what();
      std::this_thread::sleep_for(std::chrono::seconds(1));
    } catch (Poco::Net::NetException& e) {
      RAY_LOG_ERR << e.what();
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }
}