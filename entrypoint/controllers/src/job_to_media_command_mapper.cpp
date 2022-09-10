// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <Poco/File.h>
#include <Poco/Path.h>
#include <cereal/archives/json.hpp>
#include <fstream>
#include <logutil/logging.h>

#include "job_to_media_command_mapper.h"

JobToMediaCommandMapper& JobToMediaCommandMapper::get_instance()
{
  static JobToMediaCommandMapper instance;
  return instance;
}

void JobToMediaCommandMapper::set_base_dir(std::string _base_dir)
{
  JobToMediaCommandMapper::get_instance()._base_dir = std::move(_base_dir);
}
void JobToMediaCommandMapper::set_file_name(std::string _file_name)
{
  JobToMediaCommandMapper::get_instance()._file_name = std::move(_file_name);
}

void JobToMediaCommandMapper::load()
{
  Poco::Path path(_base_dir);
  path.append(_file_name);
  int ret = 0;
  std::ifstream fs(path.toString(), std::ios::in);
  if (fs.good()) {
    try {
      cereal::YAMLInputArchive archive(fs);
      archive >> map;
    } catch (std::exception& e) {
      ret++;
      RAY_LOG_INF << e.what();
    }
    fs.close();
  }

  if (map.map.empty()) {
    ret++;
  }
  if (ret > 0) {
    load_defaults();
    save_defaults();
  }
}
void JobToMediaCommandMapper::load_defaults()
{
  std::string search_str = "server";
  if (_file_name.compare(0, search_str.size(), search_str) == 0) {
    Job job;
    job.channel_id = "1";
    MediaCommand media_command;
    media_command.command = "media_converter";
    media_command.input = "rtmp://0.0.0.0:9001";
    media_command.output = "./videos/1/play.m3u8";
    media_command.timeout = fmt::format("{}", 90);

    map.map.emplace(job, media_command);
  } else {
    Job job;
    job.channel_id = "1";
    MediaCommand media_command;
    media_command.command = "media_converter";
    media_command.input = "rtsp://admin:AdmiN1234@192.168.0.58/h264/ch1/main/";
    media_command.output = "rtmp://localhost:9001";
    media_command.timeout = fmt::format("{}", 10);
    map.map.emplace(job, media_command);
  }
}
void JobToMediaCommandMapper::save_defaults()
{
  Poco::Path path(_base_dir);
  path.append(_file_name);
  std::ofstream fs(path.toString(), std::ios::out);
  if (fs.good()) {
    try {
      cereal::YAMLOutputArchive archive(fs);
      archive(CEREAL_NVP(map));
      fs.flush();
    } catch (std::exception& e) {
      RAY_LOG_INF << e.what();
    }
    fs.close();
  }
}

MediaCommand JobToMediaCommandMapper::get_media_command(const Job& job)
{
  if (JobToMediaCommandMapper::get_instance().map.map.empty()) {
    JobToMediaCommandMapper::get_instance().load();
  }
  return JobToMediaCommandMapper::get_instance().map.map[job];
}

JobMediaCommandMap JobToMediaCommandMapper::get_map()
{
  if (JobToMediaCommandMapper::get_instance().map.map.empty()) {
    JobToMediaCommandMapper::get_instance().load();
  }
  return JobToMediaCommandMapper::get_instance().map;
}