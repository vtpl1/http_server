// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************
#include <sstream>

#include "method_not_supported_request_handler.h"

void MethodNotSupportedRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& /*request*/,
                                                     Poco::Net::HTTPServerResponse& response)
{
  response.setContentType("text/plain");
  response.setStatus(Poco::Net::HTTPResponse::HTTP_METHOD_NOT_ALLOWED);
  const std::string str = Poco::Net::HTTPResponse::HTTP_REASON_METHOD_NOT_ALLOWED;
  response.setReason(str);
  response.setContentLength(static_cast<std::streamsize>(str.length()));
  response.send() << str;
}
