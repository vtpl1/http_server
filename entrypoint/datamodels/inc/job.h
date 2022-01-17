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
  Job(const std::string& job_mode, const std::string& channel_id);
  ~Job() = default;
  int16_t id{0};
  JobState job_state{JobState::IDLE};
  std::string channel_id{};
  std::string input{};
  std::string output{};
  // bool operator==(const Job& job);
  bool compare(const Job& other) const;
  // <
  // >
  // <=
  // >=
};
inline bool operator==(const Job& lhs, const Job& rhs) { return lhs.compare(rhs); }

#endif // job_h
