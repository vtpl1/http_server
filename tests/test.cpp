#include <Poco/Exception.h>
#include <Poco/File.h>
#include <catch2/catch.hpp>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>

#include "job.h"
#include "pipeline.h"

TEST_CASE("wrong command should return false", "[pipeline]")
{
  std::cout << "Started\n";
  std::unique_ptr<Pipeline> pipeline = std::make_unique<Pipeline>("temp");
  pipeline->start();
  REQUIRE_FALSE(pipeline->is_running());
  REQUIRE_FALSE(pipeline->is_running());
  REQUIRE_FALSE(pipeline->is_running());
  std::this_thread::sleep_for(std::chrono::seconds(5));
  std::cout << "End\n";
}

TEST_CASE("spawned command should return true", "[pipeline]")
{
  std::chrono::time_point<std::chrono::system_clock> entry_time = std::chrono::system_clock::now();
  std::cout << "Started\n";
  std::unique_ptr<Pipeline> pipeline = std::make_unique<Pipeline>("./HttpServer.exe");
  pipeline->start();
  REQUIRE(pipeline->is_running());
  REQUIRE(pipeline->is_running());
  REQUIRE(pipeline->is_running());
  std::this_thread::sleep_for(std::chrono::seconds(1));
  pipeline->stop();
  std::cout << "End\n";
  std::chrono::time_point<std::chrono::system_clock> exit_time = std::chrono::system_clock::now();
  std::cout << "time elapsed: " << std::chrono::duration_cast<std::chrono::seconds>(exit_time - entry_time).count()
            << " seconds" << std::endl;
}

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

TEST_CASE("jobs are equal", "[jobs]")
{
  Job job1("1");
  Job job2("1");
  REQUIRE(job1 == job2);
}

TEST_CASE("file", "[files]")
{
  Poco::File file("D:\\WorkFiles\\http_server\\static_html\\favicon1.ico");

  REQUIRE_THROWS_AS(file.isFile(), Poco::FileNotFoundException);
}