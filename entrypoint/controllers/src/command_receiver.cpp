// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPMessage.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/WebSocket.h>
#include <string>

#include "command_receiver.h"

CommandReceiver::CommandReceiver()
    : _cs("localhost", 8080), _request(Poco::Net::HTTPRequest::HTTP_GET, "/ws", Poco::Net::HTTPMessage::HTTP_1_1),
      _response()
{
}

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
    try {
      Poco::Net::WebSocket ws(_cs, _request, _response);
      try {
        ws.setReceiveTimeout(Poco::Timespan(5, 0)); // Timespan(long seconds, long microseconds)
        ws.setBlocking(true);
        ws.setKeepAlive(true);

        char receiveBuff[256];
        int flags = 0;

        int rlen = ws.receiveFrame(receiveBuff, 256, flags);
        std::cout << rlen << " bytes received : " << receiveBuff[rlen] << std::endl;
      } catch (Poco::TimeoutException& e) {
        char const *testStr="Hello echo websocket!";
        int slen = ws.sendFrame(testStr, strlen(testStr), Poco::Net::WebSocket::FRAME_OP_PING);
        std::cout << slen << " bytes sent" << std::endl;
      } catch (Poco::Exception& e) {
        std::cout << "Exception " << e.what();
        ws.close();
      }
    } catch (std::exception& e) {
      std::cout << "Exception " << e.what();
    }
  }
}