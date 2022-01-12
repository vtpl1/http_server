// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************
#include <Poco/File.h>
#include <Poco/FileStream.h>
#include <Poco/StreamCopier.h>

#include "file_request_handler.h"
#include "logging.h"

FileRequestHandler::FileRequestHandler(std::string base_path, std::string content_type)
    : _base_path(std::move(base_path)), _content_type(std::move(content_type))
{
}
void FileRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
  try {
    response.setContentType(_content_type);
    Poco::File request_file(_base_path);
    Poco::FileInputStream finputstr(request_file.path());
    std::streamsize fileSize = request_file.getSize();
    response.setContentLength(fileSize);
    std::ostream& ostr = response.send();
    Poco::StreamCopier::copyStream(finputstr, ostr);
    if (ostr.good()) {
      ostr.flush();
    }
    finputstr.close();
  } catch (const std::exception& e) {
    response.setContentType("text/plain");
    response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
    const std::string str = Poco::Net::HTTPResponse::HTTP_REASON_INTERNAL_SERVER_ERROR;
    response.setReason(str);
    response.setContentLength(str.length());
    response.send() << str;
    RAY_LOG_ERR << e.what() << " " << _base_path;
  }
}