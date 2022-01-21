// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <Poco/Net/NetException.h>
#include <cereal/archives/binary.hpp>
#include <sstream>

#include "function_request_data.h"
#include "function_request_or_response_data.h"
#include "function_response_data.h"
#include "logging.h"
#include "rpc_handler.h"
#include "rpc_manager.h"

constexpr int MAX_BUFFER_SIZE = 1024;
constexpr int RECEIVE_TIMEOUT_MICRO_SEC = 500 * 1000;
constexpr int PING_MINIMUM_INTERVAL_SEC = 10;
constexpr int PING_PONG_GAP_SEC = 2;

RpcHandler::RpcHandler(Poco::Net::WebSocket& web_socket, bool send_periodic_ping)
    : _web_socket(web_socket), _send_periodic_ping(send_periodic_ping)
{
  _web_socket.setReceiveTimeout(
      Poco::Timespan(0, RECEIVE_TIMEOUT_MICRO_SEC)); // Timespan(long seconds, long microseconds)
  _web_socket.setMaxPayloadSize(MAX_BUFFER_SIZE);
  _buffer.resize(MAX_BUFFER_SIZE);
}

bool RpcHandler::sendData(Poco::Net::WebSocket::FrameOpcodes flags)
{
  std::vector<uint8_t> buffer;
  return sendData(buffer, flags);
}
bool RpcHandler::sendData(std::vector<uint8_t>& buffer)
{
  return sendData(buffer, Poco::Net::WebSocket::FRAME_OP_BINARY);
}

bool RpcHandler::sendData(std::vector<uint8_t>& buffer, Poco::Net::WebSocket::FrameOpcodes flags)
{
  // Poco::Net::WebSocket::FRAME_OP_PONG
  try {
    int n = _web_socket.sendFrame(buffer.data(), buffer.size(), flags);
    if (n != buffer.size()) {
      return false;
    }
    _last_op_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::high_resolution_clock::now().time_since_epoch())
                        .count();
  } catch (Poco::Net::NetException& e) {
    RAY_LOG_ERR << e.what();
    return false;
  }
  return true;
}
bool RpcHandler::do_next()
{
  int n = 0;
  int flags = 0;
  bool is_timeout = false;
  try {
    flags = 0;
    n = _web_socket.receiveFrame(_buffer.data(), static_cast<int>(_buffer.size()), flags);
    // RAY_LOG_INF << Poco::format("Frame received (length=%d, flags=0x%x).", n, unsigned(flags));
  } catch (Poco::TimeoutException& e) {
    is_timeout = true;
  } catch (Poco::Net::WebSocketException& e) {
    RAY_LOG_ERR << e.what();
    return false;
  } catch (Poco::Net::NetException& e) {
    RAY_LOG_ERR << e.what();
    return false;
  }
  if ((n == 0) && (flags == 0) && !is_timeout) {
    return false;
  }
  auto ui_flags = static_cast<unsigned int>(flags);
  if (ui_flags > 0) {
    if (((ui_flags & Poco::Net::WebSocket::FRAME_OP_BITMASK) == Poco::Net::WebSocket::FRAME_OP_CLOSE)) {
      std::cout << "close request received" << std::endl;
      return false;
    }
    if (((ui_flags & Poco::Net::WebSocket::FRAME_OP_BITMASK) == Poco::Net::WebSocket::FRAME_OP_PING)) {
      return sendData(Poco::Net::WebSocket::FRAME_OP_PONG);
    }
    if (((ui_flags & Poco::Net::WebSocket::FRAME_OP_BITMASK) == Poco::Net::WebSocket::FRAME_OP_PONG)) {
      _pong_received = true;
    }
  }
  if (_send_periodic_ping) {
    int64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::high_resolution_clock::now().time_since_epoch())
                      .count();
    if ((now - _last_ping_time) >= PING_PONG_GAP_SEC) {
      if (!_pong_received) {
        RAY_LOG_ERR << "Pong not received on time";
        return false;
      }
    }
    if ((now - _last_op_time) >= PING_MINIMUM_INTERVAL_SEC) {
      _last_ping_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::high_resolution_clock::now().time_since_epoch())
                            .count();
      _pong_received = false;
      return sendData(Poco::Net::WebSocket::FRAME_OP_PING);
    }
  }
  if (n == 0) {
    RAY_LOG_ERR << "Unexpected here";
    return false;
  }
  FunctionRequestOrResponseData function_request_or_response_data;
  {
    std::stringstream ss;
    std::copy(_buffer.begin(), _buffer.begin() + n, std::ostream_iterator<uint8_t>(ss));
    {
      cereal::BinaryInputArchive iarchive(ss);
      iarchive >> function_request_or_response_data;
    }
  }
  if (function_request_or_response_data.request_or_response ==
      FunctionRequestOrResponseData::RequestOrResponse::REQUEST) {
    FunctionRequestData req;
    {
      int n = function_request_or_response_data.data.size();
      std::stringstream ss;
      std::copy(function_request_or_response_data.data.begin(), function_request_or_response_data.data.begin() + n,
                std::ostream_iterator<uint8_t>(ss));
      {
        cereal::BinaryInputArchive iarchive(ss);
        iarchive >> req;
      }
    }
    RpcManager::call_function(req.func_name, req.args);
  } else if (function_request_or_response_data.request_or_response ==
             FunctionRequestOrResponseData::RequestOrResponse::RESPONSE) {
    FunctionResponseData req;
    {
      int n = function_request_or_response_data.data.size();
      std::stringstream ss;
      std::copy(function_request_or_response_data.data.begin(), function_request_or_response_data.data.begin() + n,
                std::ostream_iterator<uint8_t>(ss));
      {
        cereal::BinaryInputArchive iarchive(ss);
        iarchive >> req;
      }
    }
    RpcManager::call_reponse(req.func_name, req.ret);
  } else {
    RAY_LOG_ERR << "Unexpected";
  }
  bool is_continue = false;
  do {
    std::unique_ptr<FunctionRequestData> function_request_data = RpcManager::get_remote_callable_function();
    std::vector<uint8_t> send_buffer;
    FunctionRequestOrResponseData function_request_or_response_data;

    if (function_request_data) {
      std::stringstream ss;
      {
        cereal::BinaryOutputArchive oarchive(ss);
        oarchive << CEREAL_NVP(*function_request_data);
      }
      std::string s = ss.str();
      std::copy(s.begin(), s.end(), std::back_inserter(function_request_or_response_data.data));
      function_request_or_response_data.request_or_response = FunctionRequestOrResponseData::RequestOrResponse::REQUEST;
      is_continue = true;
    }
    std::unique_ptr<FunctionResponseData> function_response_data = RpcManager::get_remote_callable_response();
    if (function_response_data) {
      std::stringstream ss;
      {
        cereal::BinaryOutputArchive oarchive(ss);
        oarchive << CEREAL_NVP(*function_response_data);
      }
      std::string s = ss.str();
      std::copy(s.begin(), s.end(), std::back_inserter(function_request_or_response_data.data));
      function_request_or_response_data.request_or_response =
          FunctionRequestOrResponseData::RequestOrResponse::RESPONSE;
      is_continue = true;
    }
    std::stringstream ss;
    {
      cereal::BinaryOutputArchive oarchive(ss);
      oarchive << CEREAL_NVP(function_request_or_response_data);
    }

    std::string s = ss.str();
    std::copy(s.begin(), s.end(), std::back_inserter(send_buffer));
    if (!sendData(send_buffer)) {
      return false;
    }
  } while (is_continue);

  return true;
}