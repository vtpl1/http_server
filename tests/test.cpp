#include <catch2/catch.hpp>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>

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
  std::cout << "Started\n";
  std::unique_ptr<Pipeline> pipeline = std::make_unique<Pipeline>("D:\\WorkFiles\\http_server\\build\\processes\\Debug\\process_exit_after.exe");
  pipeline->start();
  REQUIRE(pipeline->is_running());
  REQUIRE(pipeline->is_running());
  REQUIRE(pipeline->is_running());
  std::this_thread::sleep_for(std::chrono::seconds(5));
  std::cout << "End\n";
}