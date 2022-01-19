// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef job_h
#define job_h

#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <iostream>
#include <stdint.h>
#include <string>

enum class JobState { IDLE, RUNNING, END };

class Job
{
public:
  Job() = default;
  Job(std::string channel_id);
  ~Job() = default;
  std::string channel_id{};
  bool equals_to(const Job& other) const;
  bool less_than(const Job& other) const;
  // bool greater_than(const Job& other) const;
  // bool less_than_equals_to(const Job& other) const;
  // bool greater_than_equals_to(const Job& other) const;

  template <class Archive> void serialize(Archive& archive) { archive(CEREAL_NVP(channel_id)); }
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
inline std::ostream& operator<<(std::ostream& out, const Job& job)
{
  out << job.channel_id;
  return out;
}

#endif // job_h
