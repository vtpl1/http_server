// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************
#include <logutil/logging.h>
#include <sstream>

#include "not_found_request_handler.h"

void NotFoundRequestHandler::staticHandleRequest(Poco::Net::HTTPServerRequest& request,
                                                 Poco::Net::HTTPServerResponse& response)
{
  RAY_LOG_INF << "Not found : " << request.getURI();
  response.setContentType("text/plain");
  response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
  const std::string str = Poco::Net::HTTPResponse::HTTP_REASON_NOT_FOUND;
  response.setReason(str);
  response.setContentLength(static_cast<std::streamsize>(str.length()));
  response.send() << str;
}
void NotFoundRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request,
                                           Poco::Net::HTTPServerResponse& response)
{
  staticHandleRequest(request, response);
}
