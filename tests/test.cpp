#include <Poco/Exception.h>
#include <Poco/File.h>
#include <catch2/catch.hpp>
#include <cctype>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>

#include "job.h"
// #include "job.pb.h"
#include "job_to_media_command_mapper.h"
#include "pipeline.h"
#include "status.h"
// #include "status.pb.h"

// TEST_CASE("wrong command should return false", "[pipeline]")
// {
//   std::cout << "Started\n";
//   std::unique_ptr<Pipeline> pipeline = std::make_unique<Pipeline>("temp");
//   pipeline->start();
//   REQUIRE_FALSE(pipeline->is_running());
//   REQUIRE_FALSE(pipeline->is_running());
//   REQUIRE_FALSE(pipeline->is_running());
//   std::this_thread::sleep_for(std::chrono::seconds(5));
//   std::cout << "End\n";
// }

// TEST_CASE("spawned command should return true", "[pipeline]")
// {
//   std::chrono::time_point<std::chrono::system_clock> entry_time = std::chrono::system_clock::now();
//   std::cout << "Started\n";
//   std::unique_ptr<Pipeline> pipeline = std::make_unique<Pipeline>("./HttpServer.exe");
//   pipeline->start();
//   REQUIRE(pipeline->is_running());
//   REQUIRE(pipeline->is_running());
//   REQUIRE(pipeline->is_running());
//   std::this_thread::sleep_for(std::chrono::seconds(1));
//   pipeline->stop();
//   std::cout << "End\n";
//   std::chrono::time_point<std::chrono::system_clock> exit_time = std::chrono::system_clock::now();
//   std::cout << "time elapsed: " << std::chrono::duration_cast<std::chrono::seconds>(exit_time - entry_time).count()
//             << " seconds" << std::endl;
// }

// TEST_CASE("spawned command should return true once again", "[pipeline]")
// {
//   std::cout << "Started\n";
//   std::unique_ptr<Pipeline> pipeline =
//   std::make_unique<Pipeline>("F:\\WorkFiles\\http_server\\build\\processes\\Debug\\process_exit_properly.exe");
//   pipeline->start();
//   REQUIRE(pipeline->is_running());
//   REQUIRE(pipeline->is_running());
//   REQUIRE(pipeline->is_running());
//   std::this_thread::sleep_for(std::chrono::seconds(1));
//   pipeline->stop();
//   std::cout << "End\n";
// }

// TEST_CASE("jobs are equal", "[jobs]")
// {
//   Job job1("SERVER", "1");
//   Job job2("SERVER", "1");
//   REQUIRE(job1 == job2);
// }

// TEST_CASE("file", "[files]")
// {
//   Poco::File file("D:\\WorkFiles\\http_server\\static_html\\favicon1.ico");

//   REQUIRE_THROWS_AS(file.isFile(), Poco::FileNotFoundException);
// }

// TEST_CASE("job serialize", "[jobs]")
// {
//   JobList job_list;
//   Job job1;
//   job_list.push_back(job1);
//   Job job2;
//   job_list.push_back(job2);

//   std::stringstream ss; // any stream can be used
//   {
//     cereal::BinaryOutputArchive oarchive(ss); // Create an output archive
//     // oarchive(CEREAL_NVP(job1));
//     // oarchive(job1);
//     oarchive << job_list;
//   }
//   std::cout << ss.str();
//   JobList job_list2;
//   ss.seekp(0);
//   {
//     cereal::BinaryInputArchive iarchive(ss);
//     iarchive >> job_list2;
//   }
//   REQUIRE(job1 == job_list2[0]);
//   std::stringstream ss1;
//   {
//     cereal::JSONOutputArchive oarchive_json(ss1);
//     oarchive_json(CEREAL_NVP(job_list2));
//   }
//   std::cout << std::endl << ss1.str() << std::endl;
// }

// TEST_CASE("Media command serialize", "[media_command]")
// {
//   std::stringstream ss;
//   {
//     // cereal::JSONOutputArchive oarchive_json(ss);
//     cereal::YAMLOutputArchive archive(ss);
//     archive(JobToMediaCommandMapper::get_map());
//   }
//   std::cout << std::endl << ss.str() << std::endl;
//   ss.seekp(0);
//   JobMediaCommandMap map;
//   {
//     cereal::YAMLInputArchive iarchive(ss);
//     iarchive >> map;
//   }
// }

TEST_CASE("Media command persistance", "[media_command]")
{
  JobToMediaCommandMapper::set_base_dir("session");
  JobToMediaCommandMapper::set_file_name("server.yml");
  std::stringstream ss;
  {
    // cereal::JSONOutputArchive oarchive_json(ss);
    cereal::YAMLOutputArchive archive(ss);
    archive(JobToMediaCommandMapper::get_map());
  }
  std::cout << std::endl << ss.str() << std::endl;
}