// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef rpc_handler_h
#define rpc_handler_h
#include <Poco/Net/WebSocket.h>
#include <vector>

class RpcHandler
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
  bool processRecivedData(int n);
  bool processDataToSend();
  bool sendData(Poco::Net::WebSocket::FrameOpcodes flags);
  bool sendData(std::vector<uint8_t>& buffer);
  bool sendData(std::vector<uint8_t>& buffer, Poco::Net::WebSocket::FrameOpcodes flags);

public:
  RpcHandler(Poco::Net::WebSocket& web_socket, bool send_periodic_ping = false);
  ~RpcHandler() = default;
  bool do_next();
};

#endif // rpc_handler_h
