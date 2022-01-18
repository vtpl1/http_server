// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef job_h
#define job_h
#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <stdint.h>
#include <string>

enum class JobState { IDLE, RUNNING, END };

class Job
{
public:
  Job() = default;
  Job(const std::string& job_mode, const std::string& channel_id);
  ~Job() = default;
  int16_t id{0};
  JobState job_state{JobState::IDLE};
  std::string channel_id{};
  std::string input{};
  std::string output{};
  std::string remote_input{};
  std::string remote_output{};
  bool equals_to(const Job& other) const;
  bool less_than(const Job& other) const;
  // bool greater_than(const Job& other) const;
  // bool less_than_equals_to(const Job& other) const;
  // bool greater_than_equals_to(const Job& other) const;

  template <class Archive> void serialize(Archive& archive)
  {
    archive(id, channel_id, input, output, remote_input,
            remote_output); // serialize things by passing them to the archive
  }
};
inline bool operator==(const Job& lhs, const Job& rhs) { return lhs.equals_to(rhs); }
inline bool operator<(const Job& lhs, const Job& rhs) { return lhs.less_than(rhs); }
// inline bool operator>(const Job& lhs, const Job& rhs) { return lhs.greater_than(rhs); }
// inline bool operator<=(const Job& lhs, const Job& rhs) { return lhs.less_than_equals_to(rhs); }
// inline bool operator>=(const Job& lhs, const Job& rhs) { return lhs.greater_than_equals_to(rhs); }

#endif // job_h
