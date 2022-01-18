// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef job_media_command_map_h
#define job_media_command_map_h
#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>
#include <map>

#include "job.h"
#include "media_command.h"

class JobMediaCommandMap
{
public:
  JobMediaCommandMap() = default;
  ~JobMediaCommandMap() = default;
  std::map<Job, MediaCommand> map;
  template <class Archive> void serialize(Archive& archive) { archive(CEREAL_NVP(map)); }
};

#endif // job_media_command_map_h
