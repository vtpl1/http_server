// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include "job.h"

// Job::Job(const std::string& job_mode, const std::string& channel_id) : channel_id(channel_id)
// {
//   id = std::stoi(channel_id);
//   if (job_mode == "SERVER") {
//     input = "rtmp://localhost:900" + channel_id;
//     output = "videos/" + channel_id + "/play.m3u8";
//   } else if (job_mode == "CLIENT") {
//     input = "rtsp://admin:AdmiN1234@192.168.0.58/h264/ch1/main/";
//     output = "rtmp://localhost:900" + channel_id;
//   }
// }
bool Job::equals_to(const Job& other) const { return (channel_id == other.channel_id); }
bool Job::less_than(const Job& other) const { return (channel_id < other.channel_id); }

// bool Job::equals_to(const Job& other) const { return (input == other.input && output == other.output); }
// bool Job::less_than(const Job& other) const { return (input < other.input && output < other.output); }
// bool Job::greater_than(const Job& other) const { return (input > other.input && output > other.output); }
// bool Job::less_than_equals_to(const Job& other) const { return (input <= other.input && output <= other.output); }
// bool Job::greater_than_equals_to(const Job& other) const { return (input >= other.input && output >= other.output); }
