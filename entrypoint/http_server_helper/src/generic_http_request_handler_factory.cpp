// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************
#include <Poco/File.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Path.h>
#include <regex>

#include "file_request_handler.h"
#include "generic_http_request_handler_factory.h"
#include "logging.h"
#include "method_not_supported_request_handler.h"
#include "not_found_request_handler.h"
#include "options_request_handler.h"
#include "web_socket_page_request_handler.h"
#include "web_socket_request_handler.h"

GenericHttpRequestHandlerFactory::GenericHttpRequestHandlerFactory(
    std::map<std::string, std::string> base_dirs, std::map<std::string, std::string> file_extension_and_mimetype_map,
    std::map<std::string, int> pattern_to_delay_map,
    std::map<std::string, std::function<void(const std::string&)>> pattern_to_callback_map)
    : _base_dirs(std::move(base_dirs)), _file_extension_and_mimetype_map(std::move(file_extension_and_mimetype_map)),
      _pattern_to_delay_map(std::move(pattern_to_delay_map)),
      _pattern_to_callback_map(std::move(pattern_to_callback_map)), _server_stopped_event(new ServerStoppedEvent())
{
}

Poco::Net::HTTPRequestHandler*
GenericHttpRequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest& request)
{
  RAY_LOG_INF << "Request from " + request.clientAddress().toString() + ": " + request.getMethod() + " " +
                     request.getURI() + " " + request.getVersion();

  for (const auto& it : request) {
    RAY_LOG_INF << it.first + ": " + it.second;
  }
  if (request.find("Upgrade") != request.end() && Poco::icompare(request["Upgrade"], "websocket") == 0) {
    return new WebSocketRequestHandler(_server_stopped_event);
  }
  if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_OPTIONS) {
    return new OptionsRequestHandler();
  }
  if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET) {
    auto const req_uri = std::string(request.getURI());
    const std::string ws_path = "/ws";
    if (req_uri.compare(0, ws_path.size(), ws_path) == 0) {
      return new WebSocketPageRequestHandler();
    }
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
    auto const req_uri = std::string(req.getURI());
    if (req_uri.compare(0, entry.first.size(), entry.first) == 0) {
      std::string sub_path = "/" + req_uri.substr(entry.first.size());
      if (is_valid_path(sub_path)) {
        auto path = entry.second + sub_path;
        if (path.back() == '/') {
          path += "index.html";
        }
        Poco::Path path1(path);
        std::string ext = path1.getExtension();
        std::string content_type = "text/plain";
        auto it = _file_extension_and_mimetype_map.find(ext);
        if (it != _file_extension_and_mimetype_map.end()) {
          content_type = it->second;
        }
        int delay_val = 0;
        for (auto&& kv : _pattern_to_delay_map) {
          auto const regex = std::regex(kv.first);
          if (std::regex_search(req_uri, regex)) {
            delay_val = kv.second;
            // RAY_LOG_INF << "Adding delay in serving: " << req_uri << " sec: " << delay_val;
            break;
          }
        }
        for (auto&& kv : _pattern_to_callback_map) {
          auto const regex = std::regex(kv.first);
          if (std::regex_search(req_uri, regex)) {
            kv.second(req_uri);
            break;
          }
        }
        return new FileRequestHandler(path1.toString(), content_type, _server_stopped_event, delay_val);
      }
    }
  }
  return new NotFoundRequestHandler();
}

void GenericHttpRequestHandlerFactory::signal_to_stop() { _server_stopped_event->signal_to_stop(); }
