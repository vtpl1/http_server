// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef job_h
#define job_h
#include <stdint.h>
#include <string>

enum class JobState { IDLE, RUNNING, END };

class Job
{
public:
  Job() = default;
  ~Job() = default;
  int16_t id{0};
  JobState job_state{JobState::IDLE};
  std::string channel_id{};
  std::string input{};
  std::string output{};
};

#endif // job_h
