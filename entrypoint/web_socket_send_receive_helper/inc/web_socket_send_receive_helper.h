// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************
#pragma once
#ifndef web_socket_send_receive_helper_h
#define web_socket_send_receive_helper_h


#include <Poco/Net/WebSocket.h>
#include <vector>

class WebSocketSendReceiveHelper
{
private:
  Poco::Net::WebSocket& _web_socket;
  std::vector<uint8_t> _buffer;
  bool _send_periodic_ping;
  bool _pong_received{true};
  int64_t _last_op_time{0};
  int64_t _last_ping_time{0};
  bool readData(int& n, int& flags);
  bool processPingPong(int& flags);
  bool sendData(Poco::Net::WebSocket::FrameOpcodes flags);
  bool sendData(std::vector<uint8_t>& buffer, unsigned flags);

public:
  WebSocketSendReceiveHelper(Poco::Net::WebSocket& web_socket, bool send_periodic_ping = false);
  ~WebSocketSendReceiveHelper() = default;
  bool readDataAndprocessPingPong(int& n);
  bool sendData(std::vector<uint8_t>& buffer);
  std::vector<uint8_t> get_buffer();
};

#endif	// web_socket_send_receive_helper_h
