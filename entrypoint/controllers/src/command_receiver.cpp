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
constexpr int TIMEOUT_SEC = 5;

CommandReceiver::CommandReceiver() {}

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
  while (!_do_shutdown_composite()) {
    std::unique_ptr<Poco::Net::WebSocket> ws;
    try {
      Poco::Net::HTTPClientSession cs("localhost", 8080);
      Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, "/ws", Poco::Net::HTTPMessage::HTTP_1_1);
      Poco::Net::HTTPResponse response;
      ws = std::make_unique<Poco::Net::WebSocket>(cs, request, response);
      ws->setReceiveTimeout(Poco::Timespan(TIMEOUT_SEC, 0)); // Timespan(long seconds, long microseconds)
      ws->setBlocking(true);
      ws->setKeepAlive(true);

    } catch (Poco::Net::WebSocketException& e) {
      RAY_LOG_INF << "Exception " << e.what();
    }
    if (ws == nullptr) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      continue;
    }
    std::array<char, MAX_BUFFER_SIZE> buffer{};
    int flags = 0;
    while (!_do_shutdown_composite()) {
      bool is_Send_ping = false;
      try {
        int rlen = ws->receiveFrame(buffer.data(), sizeof(buffer), flags);
        if (rlen >= 0) {
          buffer[rlen] = '\0';
        }
        RAY_LOG_INF << rlen << " bytes received : ";
        RAY_LOG_INF << (((flags & Poco::Net::WebSocket::FRAME_OP_BITMASK) == Poco::Net::WebSocket::FRAME_OP_PING)
                            ? "PING"
                            : "NO PING");
        RAY_LOG_INF << (((flags & Poco::Net::WebSocket::FRAME_OP_BITMASK) == Poco::Net::WebSocket::FRAME_OP_PONG)
                            ? "PONG"
                            : "NO PONG");
      } catch (Poco::TimeoutException& e) {
        is_Send_ping = true;
      } catch (Poco::Net::NetException& e) {
        RAY_LOG_INF << "Exception " << e.what();
        break;
      }
      if (is_Send_ping) {
        int slen = ws->sendFrame(buffer.data(), 0, Poco::Net::WebSocket::FRAME_OP_PING);
        RAY_LOG_INF << slen << " bytes sent ping";
      }
    }
    try {
      ws->close();
    } catch (Poco::Exception& e) {
      RAY_LOG_INF << "Exception " << e.what();
    }
  }
}