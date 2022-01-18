// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef media_command_h
#define media_command_h
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <string>

class MediaCommand
{
public:
  MediaCommand() = default;
  ~MediaCommand() = default;
  std::string command{};
  std::string input{};
  std::string output{};
  template <class Archive> void serialize(Archive& archive)
  {
    archive(CEREAL_NVP(command), CEREAL_NVP(input), CEREAL_NVP(output));
  }
};

#endif // media_command_h
