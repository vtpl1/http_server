// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <Poco/Net/NetException.h>
#include <Poco/Net/WebSocket.h>
#include <array>

#include "logging.h"
#include "web_socket_request_handler.h"

constexpr int MAX_BUFFER_SIZE = 1024;
constexpr int TIMEOUT_SEC = 5;
WebSocketRequestHandler::WebSocketRequestHandler(ServerStoppedEvent::Ptr server_stopped_event)
    : PocoNetStoppableHTTPRequestHandler(std::move(server_stopped_event))
{
}

void WebSocketRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request,
                                            Poco::Net::HTTPServerResponse& response)
{
  try {
    Poco::Net::WebSocket ws(request, response);
    ws.setReceiveTimeout(Poco::Timespan(TIMEOUT_SEC, 0)); // Timespan(long seconds, long microseconds)
    ws.setBlocking(true);
    ws.setKeepAlive(true);

    RAY_LOG_INF << "WebSocket connection established.";
    std::array<char, MAX_BUFFER_SIZE> buffer{};
    int flags = 0;
    int n = 0;
    do {
      try {
        n = ws.receiveFrame(buffer.data(), sizeof(buffer), flags);
        RAY_LOG_INF << Poco::format("Frame received (length=%d, flags=0x%x).", n, unsigned(flags));
        // ws.sendFrame(buffer.data(), n, flags);
        std::cout << (((flags & Poco::Net::WebSocket::FRAME_OP_BITMASK) == Poco::Net::WebSocket::FRAME_OP_PING)
            ? "PING"
            : "NO PING");
        std::cout << (((flags & Poco::Net::WebSocket::FRAME_OP_BITMASK) == Poco::Net::WebSocket::FRAME_OP_PONG)
            ? "PONG"
            : "NO PONG");
        if (n == 0) {
          if ((flags & Poco::Net::WebSocket::FRAME_OP_BITMASK) == Poco::Net::WebSocket::FRAME_OP_CLOSE) {
            break;
          }
          if ((flags & Poco::Net::WebSocket::FRAME_OP_BITMASK) == Poco::Net::WebSocket::FRAME_OP_PING) {
            ws.sendFrame(buffer.data(), 0, Poco::Net::WebSocket::FRAME_OP_PONG);
          }
        }
      } catch (Poco::TimeoutException& e) {
        std::cerr << e.what() << '\n';
      }

      // while (n > 0 && (flags & Poco::Net::WebSocket::FRAME_OP_BITMASK) != Poco::Net::WebSocket::FRAME_OP_CLOSE);
    } while (!stopped); // while (n > 0 && (flags & Poco::Net::WebSocket::FRAME_OP_BITMASK) !=
                        // Poco::Net::WebSocket::FRAME_OP_CLOSE);
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
  }
}
