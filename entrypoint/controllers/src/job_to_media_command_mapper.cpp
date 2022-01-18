// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include "job_to_media_command_mapper.h"

JobToMediaCommandMapper& JobToMediaCommandMapper::get_instance()
{
  static JobToMediaCommandMapper instance;
  return instance;
}


JobToMediaCommandMapper::JobToMediaCommandMapper()
{
    Job job;
    job.channel_id = "11";
    MediaCommand media_command;
    media_command.command = "media_converete";
    media_command.input = "rtsp://admin:Admin@123@192.168.1.1/";
    media_command.output = "./videos/play.m3u8";
    // map.map.emplace()
}
MediaCommand JobToMediaCommandMapper::get_media_command(const Job& job)
{
  return JobToMediaCommandMapper::get_instance().map.map[job];
}