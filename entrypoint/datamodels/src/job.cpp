// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include "job.h"

Job::Job(std::string job_mode, std::string channel_id) : channel_id(channel_id)
{
  id = std::stoi(channel_id);
  if (job_mode == "SERVER") {
    input = "rtmp://localhost:900" + channel_id;
    output = "videos/" + channel_id + "/play.m3u8";
  } else if (job_mode == "CLIENT") {
    input = "rtsp://admin:AdmiN1234@192.168.0.58/h264/ch1/main/";
    output = "rtmp://localhost:900" + channel_id;
  }
}
bool Job::compare(const Job& other) const
{
  if (input == other.input && output == other.output)
    return true;
  return false;
}
