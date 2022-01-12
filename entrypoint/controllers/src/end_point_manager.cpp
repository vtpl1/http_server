// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include "end_point_manager.h"
#include "logging.h"
#include <Poco/Path.h>
// https://streaming.videonetics.com/live/hls/33E2C658-8F16-408A-8523-AAD5F41CB67A_HLS_SERVER_MI/play.m3u8
//  /live/hls/{stream_id}/play.m3u8
EndPointManager::EndPointManager(JobListManager* jlm, std::string base_dir, int server_port)
    : _jlm(jlm), _base_dir(std::move(base_dir)), _server_port(server_port)
{
}
EndPointManager::~EndPointManager() { stop(); }
void EndPointManager::start() { _thread = std::make_unique<std::thread>(&EndPointManager::run, this); }
void EndPointManager::signal_to_stop()
{
  _do_shutdown = true;
  if (_svr) {
    _svr->stop();
  }
}
void EndPointManager::stop()
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
void EndPointManager::run()
{
  _svr = std::make_unique<httplib::Server>();
  _svr->set_logger([](const auto& req, const auto& res) { RAY_LOG_INF << req.path << " response: " << res.status; });
  _svr->set_error_handler([](const auto& req, auto& res) {
    RAY_LOG_ERR << req.path << " response: " << res.status;
    auto fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
    char buf[BUFSIZ];
    snprintf(buf, sizeof(buf), fmt, res.status);
    res.set_content(buf, "text/html");
    RAY_LOG_ERR << req.path << " response: " << res.status;
  });
  _svr->set_exception_handler([](const auto& req, auto& res, std::exception& e) {
    res.status = 500;
    auto fmt = "<h1>Error 500</h1><p>%s</p>";
    char buf[BUFSIZ];
    snprintf(buf, sizeof(buf), fmt, e.what());
    res.set_content(buf, "text/html");
    RAY_LOG_ERR << req.path << " response: " << res.status;
  });
  // User defined file extension and MIME type mappings
  // svr->set_file_extension_and_mimetype_mapping("cc", "text/x-c");
  // svr->set_file_extension_and_mimetype_mapping("cpp", "text/x-c");
  // svr->set_file_extension_and_mimetype_mapping("hh", "text/x-h");
  std::vector<std::string> static_pages = {"/videos", "/static_html"};
  for (auto&& url : static_pages) {
    Poco::Path _path(_base_dir);
    _path.append(url);
    RAY_LOG_INF << "serving: [" << url << "] from: " << _path.toString();
    if (!_svr->set_mount_point(url, _path.toString())) {
      RAY_LOG_ERR << url << " not found";
    }
  }
  _svr->listen("0.0.0.0", 8080);
}
