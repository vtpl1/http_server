// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPMessage.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/WebSocket.h>

#include "command_receiver.h"

CommandReceiver::CommandReceiver()
    : cs("localhost", 8080), request(Poco::Net::HTTPRequest::HTTP_GET, "/ws", Poco::Net::HTTPMessage::HTTP_1_1),
      response()
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
      Poco::Net::WebSocket ws(cs, request, response);
      char const* testStr = "Hello echo websocket!";
      char receiveBuff[256];

      int len = ws.sendFrame(testStr, strlen(testStr), Poco::Net::WebSocket::FRAME_TEXT);
      std::cout << "Sent bytes " << len << std::endl;
      int flags = 0;

      int rlen = ws.receiveFrame(receiveBuff, 256, flags);
      std::cout << "Received bytes " << rlen << std::endl;
      std::cout << receiveBuff << std::endl;

      ws.close();

    } catch (std::exception& e) {
      std::cout << "Exception " << e.what();
    }
  }
}