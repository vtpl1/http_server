// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************
#include <Poco/File.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Path.h>

#include "file_request_handler.h"
#include "generic_http_request_handler_factory.h"
#include "method_not_supported_request_handler.h"
#include "not_found_request_handler.h"
#include "options_request_handler.h"

GenericHttpRequestHandlerFactory::GenericHttpRequestHandlerFactory(
    std::map<std::string, std::string> base_dirs, std::map<std::string, std::string> file_extension_and_mimetype_map)
    : _base_dirs(std::move(base_dirs)), _file_extension_and_mimetype_map(std::move(file_extension_and_mimetype_map))
{
}

Poco::Net::HTTPRequestHandler*
GenericHttpRequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest& request)
{
  if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_OPTIONS) {
    return new OptionsRequestHandler();
  }
  if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET) {
    Poco::Net::HTTPRequestHandler* ret = handle_file_request(request);
    return ret;
  }
  return new MethodNotSupportedRequestHandler();
}

bool GenericHttpRequestHandlerFactory::is_valid_path(const std::string& path)
{
  size_t level = 0;
  size_t i = 0;

  // Skip slash
  while (i < path.size() && path[i] == '/') {
    i++;
  }

  while (i < path.size()) {
    // Read component
    auto beg = i;
    while (i < path.size() && path[i] != '/') {
      i++;
    }

    auto len = i - beg;
    if (!(len > 0)) {
      throw std::runtime_error("Here ");
    }
    // assert(len > 0);

    if (path.compare(beg, len, ".") == 0) {
      ;
    } else if (path.compare(beg, len, "..") == 0) {
      if (level == 0) {
        return false;
      }
      level--;
    } else {
      level++;
    }

    // Skip slash
    while (i < path.size() && path[i] == '/') {
      i++;
    }
  }

  return true;
}

Poco::Net::HTTPRequestHandler*
GenericHttpRequestHandlerFactory::handle_file_request(const Poco::Net::HTTPServerRequest& req)
{
  for (const auto& entry : _base_dirs) {
    // Prefix match
    if (req.getURI().compare(0, entry.first.size(), entry.first) == 0) {
      std::string sub_path = "/" + req.getURI().substr(entry.first.size());
      if (is_valid_path(sub_path)) {
        auto path = entry.second + sub_path;
        if (path.back() == '/') {
          path += "index.html";
        }
        Poco::Path path1(path);
        if (path1.isFile()) {
          std::string ext = path1.getExtension();
          std::string content_type = "text/plain";
          auto it = _file_extension_and_mimetype_map.find(ext);
          if (it != _file_extension_and_mimetype_map.end()) {
            content_type = it->second;
          }
          return new FileRequestHandler(path1.toString(), content_type);
        }
      }
    }
  }
  return new NotFoundRequestHandler();
}
