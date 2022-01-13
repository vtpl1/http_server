// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************
#include "file_request_handler.h"
#include "logging.h"
#include "not_found_request_handler.h"
#include <Poco/Exception.h>
#include <Poco/File.h>
#include <Poco/FileStream.h>
#include <Poco/StreamCopier.h>
#include <thread>

FileRequestHandler::FileRequestHandler(std::string base_path, std::string content_type,
                                       ServerStoppedEvent::Ptr server_stopped_event,
                                       int monitor_in_sec_for_availability)
    : _base_path(std::move(base_path)), _content_type(std::move(content_type)),
      PocoNetStoppableHTTPRequestHandler(server_stopped_event),
      _monitor_in_sec_for_availability(monitor_in_sec_for_availability)
{
}
void FileRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
  bool is_file_found{false};
  Poco::File request_file(_base_path);
  // auto start = std::chrono::high_resolution_clock::now();
  // do {
  //   try {
  //     if (request_file.isFile()) {
  //       is_file_found = true;
  //       break;
  //     }
  //   } catch (const Poco::FileNotFoundException& e) {
  //   }

  //   if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - start).count()
  //   >=
  //       _monitor_in_sec_for_availability) {
  //     break;
  //   }
  //   std::this_thread::sleep_for(std::chrono::seconds(1));
  //   RAY_LOG_WAR << "Waiting for " << request.getURI();
  // } while (!stopped);

  try {
    if (request_file.isFile()) {
      is_file_found = true;
    }
  } catch (const Poco::FileNotFoundException& e) {
  }

  if (!is_file_found) {
    NotFoundRequestHandler::staticHandleRequest(request, response);
    return;
  }
  try {
    response.setContentType(_content_type);
    std::string origin = "*";
    try {
      origin = request.get("Origin");
    } catch (const std::exception& e) {
    }
    response.add("Access-Control-Allow-Origin", origin);
    Poco::FileInputStream finputstr(request_file.path());
    std::streamsize fileSize = request_file.getSize();
    response.setContentLength(fileSize);
    std::ostream& ostr = response.send();
    Poco::StreamCopier::copyStream(finputstr, ostr);
    if (ostr.good()) {
      ostr.flush();
    }
    finputstr.close();
  } catch (const Poco::FileException& e) {
    NotFoundRequestHandler::staticHandleRequest(request, response);
  } catch (const Poco::Exception& e) {
    RAY_LOG_ERR << "Can NOT do much: " << e.what() << " " << _base_path;
  }
  //  catch (const std::exception& e) {
  //   response.setContentType("text/plain");
  //   response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
  //   const std::string str = Poco::Net::HTTPResponse::HTTP_REASON_INTERNAL_SERVER_ERROR;
  //   response.setReason(str);
  //   response.setContentLength(str.length());
  //   response.send() << str;
  //   RAY_LOG_ERR << e.what() << " " << _base_path;
  // }
}