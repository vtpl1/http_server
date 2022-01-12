// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include "options_request_handler.h"

void OptionsRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request,
                                          Poco::Net::HTTPServerResponse& response)
{
  response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
  response.setContentLength(0);
  response.add("Allow", "GET,POST,OPTIONS");
  std::string origin = "*";
  try {
    origin = request.get("Origin");
  } catch (const std::exception& e) {
  }
  response.add("Access-Control-Allow-Origin", origin);
  response.add("Access-Control-Allow-Credentials", "true");
  response.add("Access-Control-Allow-Methods", "GET,POST,OPTIONS");
  response.add("Access-Control-Allow-Headers", "Accept,Authorization, "
                                               "Keep-Alive,X-CustomHeader,Origin,DNT,User-Agent,X-Requested-With,If-"
                                               "Modified-Since,Cache-Control,Content-Type,Range");
  response.add("Access-Control-Expose-Headers", "Content-Length,Content-Range");
  response.add("Access-Control-Max-Age", "1728000");

  response.add("Content-Type", "text/plain charset=UTF-8");
  response.setKeepAlive(request.getKeepAlive());
  response.send();
}
