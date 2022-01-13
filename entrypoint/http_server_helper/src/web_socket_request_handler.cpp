// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <Poco/Net/NetException.h>
#include <Poco/Net/WebSocket.h>
#include <array>

#include "logging.h"
#include "web_socket_request_handler.h"

constexpr int MAX_BUFFER_SIZE = 1024;

void WebSocketRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request,
                                            Poco::Net::HTTPServerResponse& response)
{
  try {
    Poco::Net::WebSocket ws(request, response);

    RAY_LOG_INF << "WebSocket connection established.";
    std::array<char, MAX_BUFFER_SIZE> buffer{};
    int flags = 0;
    int n = 0;
    do {
      n = ws.receiveFrame(buffer.data(), sizeof(buffer), flags);
      RAY_LOG_INF << Poco::format("Frame received (length=%d, flags=0x%x).", n, unsigned(flags));
      ws.sendFrame(buffer.data(), n, flags);
    } while (n > 0 && (flags & Poco::Net::WebSocket::FRAME_OP_BITMASK) != Poco::Net::WebSocket::FRAME_OP_CLOSE);
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
