// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <Poco/Net/NetException.h>
#include <chrono>
#include <logutil/logging.h>

#include "web_socket_send_receive_helper.h"

constexpr int MAX_BUFFER_SIZE = 1024;
constexpr int RECEIVE_TIMEOUT_MICRO_SEC = 500 * 1000;
constexpr int PING_MINIMUM_INTERVAL_SEC = 10;
constexpr int PING_PONG_GAP_SEC = 2;

WebSocketSendReceiveHelper::WebSocketSendReceiveHelper(Poco::Net::WebSocket& web_socket, bool send_periodic_ping)
    : _web_socket(web_socket), _send_periodic_ping(send_periodic_ping)
{
  _web_socket.setReceiveTimeout(
      Poco::Timespan(0, RECEIVE_TIMEOUT_MICRO_SEC)); // Timespan(long seconds, long microseconds)
  _web_socket.setMaxPayloadSize(MAX_BUFFER_SIZE);
  _buffer.resize(MAX_BUFFER_SIZE);
}
bool WebSocketSendReceiveHelper::sendData(Poco::Net::WebSocket::FrameOpcodes flags)
{
  std::vector<uint8_t> buffer;
  unsigned flag = 0;
  flag |= flags;
  flag |= Poco::Net::WebSocket::FRAME_FLAG_FIN;
  // buffer.push_back(0);
  return sendData(buffer, flag);
}
bool WebSocketSendReceiveHelper::sendData(std::vector<uint8_t>& buffer)
{
  unsigned flag = 0;
  flag |= Poco::Net::WebSocket::FRAME_BINARY;
  return sendData(buffer, flag);
}

bool WebSocketSendReceiveHelper::sendString(const std::string& buffer)
{
  unsigned flag = 0;
  flag |= Poco::Net::WebSocket::FRAME_TEXT;
  return sendData(buffer.c_str(), buffer.size(), flag);
}

bool WebSocketSendReceiveHelper::sendData(std::vector<uint8_t>& buffer, unsigned flags)
{
  return sendData(buffer.data(), buffer.size(), flags);
}

bool WebSocketSendReceiveHelper::sendData(const void* buffer, int length, unsigned flags)
{
  // Poco::Net::WebSocket::FRAME_OP_PONG
  RAY_LOG_INF << "Sent: " << length << " , " << flags;
  try {
    int n = _web_socket.sendFrame(buffer, length, static_cast<int>(flags));
    if (n != length) {
      return false;
    }
    _last_op_time =
        std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now().time_since_epoch())
            .count();
  } catch (Poco::Net::NetException& e) {
    RAY_LOG_ERR << e.what();
    return false;
  }
  return true;
}

bool WebSocketSendReceiveHelper::readData(int& n, int& flags)
{
  bool is_timeout = false;
  try {
    flags = 0;
    n = _web_socket.receiveFrame(_buffer.data(), static_cast<int>(_buffer.size()), flags);
    RAY_LOG_INF << Poco::format("Frame received (length=%d, flags=0x%x).", n, unsigned(flags));
  } catch (Poco::TimeoutException& e) {
    is_timeout = true;
  } catch (Poco::Net::WebSocketException& e) {
    RAY_LOG_ERR << e.what();
    return false;
  } catch (Poco::Net::NetException& e) {
    RAY_LOG_ERR << e.what();
    return false;
  }
  return !((n == 0) && (flags == 0) && !is_timeout);
}
bool WebSocketSendReceiveHelper::processPingPong(int& flags)
{
  auto ui_flags = static_cast<unsigned int>(flags);
  if (ui_flags > 0) {
    if (((ui_flags & Poco::Net::WebSocket::FRAME_OP_BITMASK) == Poco::Net::WebSocket::FRAME_OP_CLOSE)) {
      RAY_LOG_INF << "Close Request Received";
      return false;
    }
    if (((ui_flags & Poco::Net::WebSocket::FRAME_OP_BITMASK) == Poco::Net::WebSocket::FRAME_OP_PING)) {
      RAY_LOG_INF << "Ping Received";
      return sendData(Poco::Net::WebSocket::FRAME_OP_PONG);
    }
    if (((ui_flags & Poco::Net::WebSocket::FRAME_OP_BITMASK) == Poco::Net::WebSocket::FRAME_OP_PONG)) {
      RAY_LOG_INF << "Pong Received";
      _pong_received = true;
    }
  }
  if (_send_periodic_ping) {
    int64_t now =
        std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now().time_since_epoch())
            .count();
    if ((now - _last_ping_time) >= PING_PONG_GAP_SEC) {
      if (!_pong_received) {
        RAY_LOG_ERR << "Pong not received on time";
        return false;
      }
    }
    if ((now - _last_op_time) >= PING_MINIMUM_INTERVAL_SEC) {
      _last_ping_time =
          std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now().time_since_epoch())
              .count();
      _pong_received = false;
      return sendData(Poco::Net::WebSocket::FRAME_OP_PING);
    }
  }
  return true;
}

bool WebSocketSendReceiveHelper::readDataAndprocessPingPong(int& n, bool& is_binary)
{
  int flags = 0;
  if (!readData(n, flags)) {
    return false;
  }
  is_binary = (n > 0) && (flags & Poco::Net::WebSocket::FRAME_OP_BINARY);
  if (!processPingPong(flags)) {
    return false;
  }
  return true;
}

std::vector<uint8_t> WebSocketSendReceiveHelper::get_buffer() { return _buffer; }