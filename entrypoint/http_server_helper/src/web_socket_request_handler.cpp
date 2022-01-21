// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <Poco/Net/NetException.h>
#include <array>
#include <cereal/archives/binary.hpp>
#include <memory>
#include <thread>

#include "function_request_or_response_data.h"
#include "logging.h"
#include "rpc_handler.h"
#include "web_socket_request_handler.h"

constexpr int MAX_BUFFER_SIZE = 1024;
constexpr int RECEIVE_TIMEOUT_MICRO_SEC = 500 * 1000;
constexpr int PONG_MINIMUM_INTERVAL_SEC = 8;

WebSocketRequestHandler::WebSocketRequestHandler(ServerStoppedEvent::Ptr server_stopped_event,
                                                 std::vector<StatusCallBackHandler> status_call_back_handler,
                                                 std::vector<CommandCallBackHandler> command_call_back_handler)
    : PocoNetStoppableHTTPRequestHandler(std::move(server_stopped_event)),
      _status_call_back_handler(std::move(status_call_back_handler)),
      _command_call_back_handler(std::move(command_call_back_handler))
{
  buffer.resize(MAX_BUFFER_SIZE);
}

// bool WebSocketRequestHandler::rpc_backend(Poco::Net::WebSocket& ws, std::string& request_uri)
// {
//   int flags = 0;
//   int n = 0;
//   //////////////
//   flags = 0;
//   n = 0;
//   try {
//     n = ws.receiveFrame(buffer.data(), static_cast<int>(buffer.size()), flags);
//     // RAY_LOG_INF << Poco::format("Frame received (length=%d, flags=0x%x).", n, unsigned(flags));
//   } catch (Poco::TimeoutException& e) {
//     // RAY_LOG_ERR << e.what();
//   } catch (Poco::Net::NetException& e) {
//     RAY_LOG_ERR << e.what();
//     return false;
//   }
//   if (n > 0) {
//     std::stringstream ss;
//     std::copy(buffer.begin(), buffer.begin() + n, std::ostream_iterator<uint8_t>(ss));
//     cereal::BinaryInputArchive iarchive(ss);
//     FunctionRequestOrResponseData function_request_or_response_data;
//     iarchive >> function_request_or_response_data;
//     if (function_request_or_response_data.function_request == 1) {
//       std::stringstream ss;
//       std::copy(function_request_or_response_data.data.begin(), function_request_or_response_data.data.end(),
//       std::ostream_iterator<uint8_t>(ss)); cereal::BinaryInputArchive iarchive(ss); FunctionRequestData
//       function_request_data; iarchive >> function_request_data;
//       RpcManager::call_function(function_request_data.func_name, function_request_data.args);

//       // FunctionRequestOrResponseData.data
//       //  |
//       //  |--> FunctionResponseData.args
//       //            |
//       //            |--> Status
//       //  |--> FunctionRequestData.args
//       //            |
//       //            |--> JobData

//     } else {
//       std::stringstream ss;
//       std::copy(function_request_or_response_data.data.begin(), function_request_or_response_data.data.end(),
//       std::ostream_iterator<uint8_t>(ss)); cereal::BinaryInputArchive iarchive(ss); FunctionResponseData
//       function_response_data; iarchive >> function_response_data;
//     }

//     // RpcManager::call_function();
//   }
//   if (((static_cast<unsigned int>(flags) & Poco::Net::WebSocket::FRAME_OP_BITMASK) ==
//   Poco::Net::WebSocket::FRAME_OP_PING)) {
//     if ((std::chrono::duration_cast<std::chrono::milliseconds>(
//              std::chrono::high_resolution_clock::now().time_since_epoch())
//              .count() -
//          last_op_time) >= PONG_MINIMUM_INTERVAL_SEC) {
//       try {
//         ws.sendFrame(buffer.data(), 0, Poco::Net::WebSocket::FRAME_OP_PONG);
//         last_op_time = std::chrono::duration_cast<std::chrono::milliseconds>(
//                            std::chrono::high_resolution_clock::now().time_since_epoch())
//                            .count();
//         // RAY_LOG_INF << "PONG sent";
//       } catch (Poco::Net::NetException& e) {
//         RAY_LOG_ERR << e.what();
//         return false;
//       }
//     }
//   }
//   while (std::unique_ptr<FunctionRequestData> function_request_data = RpcManager::get_remote_callable_function()) {
//   }

//   // std::vector<uint8_t> valid_data;
//   // for (auto&& handler : _status_call_back_handler) {
//   //   handler(valid_data);
//   // }
//   // for (auto&& handler : _command_call_back_handler) {
//   //   std::vector<uint8_t> command_to_send = handler(request_uri);
//   //   if (!command_to_send.empty()) {
//   //     ws.sendFrame(command_to_send.data(), static_cast<int>(command_to_send.size()),
//   //                  Poco::Net::WebSocket::FRAME_OP_TEXT);
//   //   }
//   // }
//   return !((flags & Poco::Net::WebSocket::FRAME_OP_BITMASK) == Poco::Net::WebSocket::FRAME_OP_CLOSE);
//   //////////////
//   return true;
// }
void WebSocketRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request,
                                            Poco::Net::HTTPServerResponse& response)
{
  last_op_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                     std::chrono::high_resolution_clock::now().time_since_epoch())
                     .count();
  std::string request_uri = request.getURI();
  try {
    Poco::Net::WebSocket ws(request, response);
    RAY_LOG_INF << "WebSocket connection established.";
    RpcHandler rpc_handler(ws, false);
    while (!stopped) {
      if (!rpc_handler.do_next()) {
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
