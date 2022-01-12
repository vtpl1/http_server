// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include "job.h"

Job::Job(std::string channel_id) : channel_id(channel_id) {
    id = std::stoi(channel_id);
    input = "rtmp://0.0.0.0/" + channel_id;
    output = "videos/" + channel_id + "/play.m3u8";
}
bool Job::compare(const Job& other) const
{
    if (input == other.input && output == other.output)
        return true;
    return false;
}
