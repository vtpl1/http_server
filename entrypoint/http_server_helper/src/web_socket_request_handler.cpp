// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <Poco/Net/NetException.h>
#include <Poco/Net/WebSocket.h>
#include <array>
#include <thread>

#include "logging.h"
#include "web_socket_request_handler.h"

constexpr int MAX_BUFFER_SIZE = 1024;
constexpr int RECEIVE_TIMEOUT_MILLISEC = 500;
constexpr int PONG_MINIMUM_INTERVAL_SEC = 8;

WebSocketRequestHandler::WebSocketRequestHandler(ServerStoppedEvent::Ptr server_stopped_event,
                                                 std::vector<StatusCallBackHandler> status_call_back_handler,
                                                 std::vector<CommandCallBackHandler> command_call_back_handler)
    : PocoNetStoppableHTTPRequestHandler(std::move(server_stopped_event)),
      _status_call_back_handler(std::move(status_call_back_handler)),
      _command_call_back_handler(std::move(command_call_back_handler))
{
}

void WebSocketRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request,
                                            Poco::Net::HTTPServerResponse& response)
{
  auto last_op_time = std::chrono::high_resolution_clock::now();
  try {
    Poco::Net::WebSocket ws(request, response);
    ws.setReceiveTimeout(
        Poco::Timespan(0, RECEIVE_TIMEOUT_MILLISEC * 1000)); // Timespan(long seconds, long microseconds)

    RAY_LOG_INF << "WebSocket connection established.";
    std::array<char, MAX_BUFFER_SIZE> buffer{};
    int flags = 0;
    int n = 0;
    while (!stopped) {
      flags = 0;
      n = 0;
      try {
        n = ws.receiveFrame(buffer.data(), sizeof(buffer), flags);
        std::vector<uint8_t> valid_data;
        for (auto&& handler : _status_call_back_handler) {
          handler(valid_data);
        }
        for (auto&& handler : _command_call_back_handler) {
          std::vector<uint8_t> command_to_send = handler(request.getURI());
          if (command_to_send.size()) {
            ws.sendFrame(command_to_send.data(), command_to_send.size());
          }
        }


        RAY_LOG_INF << Poco::format("Frame received (length=%d, flags=0x%x).", n, unsigned(flags));
        // ws.sendFrame(buffer.data(), n, flags);
      } catch (Poco::TimeoutException& e) {
        // RAY_LOG_ERR << e.what();
      } catch (Poco::Net::NetException& e) {
        RAY_LOG_ERR << e.what();
        break;
      }
      if ((flags & Poco::Net::WebSocket::FRAME_OP_BITMASK) == Poco::Net::WebSocket::FRAME_OP_PING) {
        if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - last_op_time)
                .count() >= PONG_MINIMUM_INTERVAL_SEC) {
          try {
            ws.sendFrame(buffer.data(), 0, Poco::Net::WebSocket::FRAME_OP_PONG);
            last_op_time = std::chrono::high_resolution_clock::now();
            RAY_LOG_INF << "PONG sent";
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
    } catch (Poco::Exception& e) {
      RAY_LOG_INF << "Exception " << e.what();
    }
    RAY_LOG_INF << "WebSocket connection closed.";

  } catch (Poco::Net::WebSocketException& exc) {
    RAY_LOG_ERR << exc.what();
    switch (exc.code()) {
    case Poco::Net::WebSocket::WS_ERR_HANDSHAKE_UNSUPPORTED_VERSION:
      response.set("Sec-WebSocket-Version", Poco::Net::WebSocket::WEBSOCKET_VERSION);
      // fallthrough
    case Poco::Net::WebSocket::WS_ERR_NO_HANDSHAKE:
    case Poco::Net::WebSocket::WS_ERR_HANDSHAKE_NO_VERSION:
    case Poco::Net::WebSocket::WS_ERR_HANDSHAKE_NO_KEY:
      response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
      response.setContentLength(0);
      response.send();
      break;
    }
  } catch (Poco::Net::NetException& e) {
    RAY_LOG_ERR << e.what();
  }
}
