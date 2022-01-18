// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef job_to_media_command_mapper_h
#define job_to_media_command_mapper_h

#include "job.h"
#include "media_command.h"
#include "job_media_command_map.h"
#include "third_party/cereal-yaml/archive/yaml.hpp"

class JobToMediaCommandMapper
{
private:
  JobToMediaCommandMapper();
  JobToMediaCommandMapper(const JobToMediaCommandMapper&) = delete;
  JobToMediaCommandMapper& operator=(const JobToMediaCommandMapper&) = delete;
  std::string _base_dir{};
  std::string _file_name{};

  JobMediaCommandMap map;
  void load();
  void load_defaults();
  void save_defaults();

  static JobToMediaCommandMapper& get_instance();


public:
  ~JobToMediaCommandMapper() = default;

  static MediaCommand get_media_command(const Job& job);
  static JobMediaCommandMap get_map();
  static void set_base_dir(std::string _base_dir);
  static void set_file_name(std::string _file_name);
};

#endif // job_to_media_command_mapper_h
