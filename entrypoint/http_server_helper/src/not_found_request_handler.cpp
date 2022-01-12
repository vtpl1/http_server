// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************
#include <sstream>

#include "not_found_request_handler.h"

void NotFoundRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request,
                                           Poco::Net::HTTPServerResponse& response)
{
  response.setContentType("text/plain");
  response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
  const std::string str = Poco::Net::HTTPResponse::HTTP_REASON_NOT_FOUND;
  response.setReason(str);
  response.setContentLength(str.length());
  response.send() << str;
}
