// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <Poco/Net/NetException.h>
#include <array>
#include <cereal/archives/binary.hpp>
#include <memory>
#include <thread>

#include "logging.h"
// #include "rpc_handler.h"
#include "rpc_manager.h"
#include "web_socket_request_handler.h"
#include "web_socket_send_receive_helper.h"

constexpr int MAX_BUFFER_SIZE = 1024;
constexpr int RECEIVE_TIMEOUT_MICRO_SEC = 500 * 1000;
constexpr int PONG_MINIMUM_INTERVAL_SEC = 8;

WebSocketRequestHandler::WebSocketRequestHandler(ServerStoppedEvent::Ptr server_stopped_event)
    : PocoNetStoppableHTTPRequestHandler(std::move(server_stopped_event))
{
}

void WebSocketRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request,
                                            Poco::Net::HTTPServerResponse& response)
{
  std::string request_uri = request.getURI();
  try {
    Poco::Net::WebSocket ws(request, response);
    RAY_LOG_INF << "WebSocket connection established.";
    WebSocketSendReceiveHelper web_socket_send_receive_helper(ws, false);
    while (!stopped) {
      int n = 0;
      if (!web_socket_send_receive_helper.readDataAndprocessPingPong(n)) {
        break;
      }
      if (n > 0) {
        std::shared_ptr<std::vector<uint8_t>> valid_buffer = std::make_shared<std::vector<uint8_t>>();
        std::vector<uint8_t> buffer = web_socket_send_receive_helper.get_buffer();
        copy(buffer.begin(), buffer.begin() + n, back_inserter(*valid_buffer));
        RpcManager::put_request_buffer(valid_buffer);
      }

      std::shared_ptr<std::vector<uint8_t>> buf = RpcManager::get_send_buffer();
      if (buf != nullptr) {
        if (!buf->empty()) {
          web_socket_send_receive_helper.sendData(*buf);
        }
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
