// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <Poco/Path.h>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <logutil/logging.h>
#include <regex>

#include "end_point_manager.h"

constexpr int CHANNEL_REQ_EXP_TIME_MILI_SEC = 30 * 1000;

// https://streaming.videonetics.com/live/hls/33E2C658-8F16-408A-8523-AAD5F41CB67A_HLS_SERVER_MI/play.m3u8
//  /live/hls/{stream_id}/play.m3u8
EndPointManager::EndPointManager(JobListManager& jlm, std::string base_dir, int server_port)
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
void EndPointManager::on_url_call_back_event(const std::string& req_url)
{
  // std::regex rgx(".*videos\\/(\\d+)\\/play\\.m3u8.*");
  std::regex rgx(R"(.*videos\/(\d+)\/play\.m3u8.*)");
  std::smatch match;

  if (std::regex_search(req_url.begin(), req_url.end(), match, rgx)) {
    std::string channel_id = match[1];

    auto it = _last_access_time_map.find(channel_id);
    if (it != _last_access_time_map.end()) {
      it->second = std::chrono::duration_cast<std::chrono::milliseconds>(
                       std::chrono::high_resolution_clock::now().time_since_epoch())
                       .count();
    } else {
      _last_access_time_map.insert(
          std::make_pair(channel_id, std::chrono::duration_cast<std::chrono::milliseconds>(
                                         std::chrono::high_resolution_clock::now().time_since_epoch())
                                         .count()));
      RAY_LOG_INF << "Request received from : " << channel_id;
      _jlm.add_job(Job(channel_id));
    }
  }
}
void EndPointManager::on_status_call_back_event(const std::vector<uint8_t>& data)
{
  if (!data.empty()) {
    RAY_LOG_INF << "Status received ";
  }
}
std::vector<uint8_t> EndPointManager::on_command_call_back_event(const std::string& req_url)
{
  std::vector<uint8_t> ret_buffer;
  JobList job_list = _jlm.get_jobs();
  std::stringstream ss;
  {
    cereal::BinaryOutputArchive oarchive(ss);
    oarchive << CEREAL_NVP(job_list);
  }
  std::string s = ss.str();
  std::copy(s.begin(), s.end(), std::back_inserter(ret_buffer));

  return ret_buffer;
}
void EndPointManager::run()
{
  _svr = std::make_unique<HttpServer>(_server_port);
  // _svr->set_logger([](const auto& req, const auto& res) { RAY_LOG_INF << req.path << " response: " << res.status; });
  // _svr->set_error_handler([](const auto& req, auto& res) {
  //   RAY_LOG_ERR << req.path << " response: " << res.status;
  //   auto fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
  //   char buf[BUFSIZ];
  //   snprintf(buf, sizeof(buf), fmt, res.status);
  //   res.set_content(buf, "text/html");
  //   RAY_LOG_ERR << req.path << " response: " << res.status;
  // });
  // _svr->set_exception_handler([](const auto& req, auto& res, std::exception& e) {
  //   res.status = 500;
  //   auto fmt = "<h1>Error 500</h1><p>%s</p>";
  //   char buf[BUFSIZ];
  //   snprintf(buf, sizeof(buf), fmt, e.what());
  //   res.set_content(buf, "text/html");
  //   RAY_LOG_ERR << req.path << " response: " << res.status;
  // });
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
      RAY_LOG_ERR << url << " ******************************* not found [FATAL ERROR]";
    }
  }
  // _svr->set_delay_for_mount_point(".m3u8", 30);
  _svr->set_url_call_back_handler(".m3u8", [this](const std::string& req_uri) { on_url_call_back_event(req_uri); });
  //_svr->listen("0.0.0.0", 8080);
  _svr->start();

  while (!_do_shutdown_composite()) {
    std::vector<std::string> temp;
    for (auto&& it : _last_access_time_map) {
      if ((std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::high_resolution_clock::now().time_since_epoch())
               .count() -
           it.second) > CHANNEL_REQ_EXP_TIME_MILI_SEC) {
        std::string channel_id = it.first;
        _jlm.delete_job(Job(channel_id));
        temp.push_back(channel_id);
      }
    }
    for (auto&& it : temp) {
      // _last_access_time_map.erase(_last_access_time_map.begin(), _last_access_time_map.find(it));
      _last_access_time_map.erase(it);
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}
