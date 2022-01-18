// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef job_h
#define job_h

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
  std::string channel_id{};
  std::string input{};
  std::string output{};
  bool equals_to(const Job& other) const;
  bool less_than(const Job& other) const;
  // bool greater_than(const Job& other) const;
  // bool less_than_equals_to(const Job& other) const;
  // bool greater_than_equals_to(const Job& other) const;

  template <class Archive> void serialize(Archive& archive)
  {
    archive(CEREAL_NVP(id), CEREAL_NVP(channel_id), CEREAL_NVP(input), CEREAL_NVP(output));
  }
};
class JobList
{
public:
  JobList() = default;
  ~JobList() = default;
  JobList(const std::vector<Job>& __x) : job_list(__x) {}
  void push_back(const Job& __x) { job_list.push_back(__x); };
  void emplace_back(const Job&& __x) { job_list.emplace_back(__x); };
  Job& operator[](int i) { return job_list[i]; };
  std::vector<Job> job_list;
  template <class Archive> void serialize(Archive& archive) { archive(CEREAL_NVP(job_list)); }
};
inline bool operator==(const Job& lhs, const Job& rhs) { return lhs.equals_to(rhs); }
inline bool operator<(const Job& lhs, const Job& rhs) { return lhs.less_than(rhs); }
// inline bool operator>(const Job& lhs, const Job& rhs) { return lhs.greater_than(rhs); }
// inline bool operator<=(const Job& lhs, const Job& rhs) { return lhs.less_than_equals_to(rhs); }
// inline bool operator>=(const Job& lhs, const Job& rhs) { return lhs.greater_than_equals_to(rhs); }

#endif // job_h
