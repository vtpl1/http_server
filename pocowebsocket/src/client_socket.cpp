#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPMessage.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/WebSocket.h"
#include <iostream>

using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPMessage;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::WebSocket;

int main(int args, char** argv)
{
  HTTPClientSession cs("localhost", 8080);
  HTTPRequest request(HTTPRequest::HTTP_GET, "/ws", HTTPMessage::HTTP_1_1);
  HTTPResponse response;

  try {

    WebSocket ws(cs, request, response);
    char const* testStr = "Hello echo websocket!";
    char receiveBuff[256];

    int len = ws.sendFrame(testStr, strlen(testStr), WebSocket::FRAME_TEXT);
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
