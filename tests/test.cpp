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
#include <type_traits> // for is_void

#include "job.h"
// #include "job.pb.h"
#include "job_to_media_command_mapper.h"
#include "pipeline.h"
#include "status.h"
// #include "status.pb.h"
#include "function_request_data.h"
#include "function_request_or_response_data.h"
#include "function_response_data.h"
#include "rpc_manager.h"

TEST_CASE("wrong command should return false", "[pipeline]")
{
  std::cout << "Started\n";
  std::unique_ptr<Pipeline> pipeline = std::make_unique<Pipeline>("temp");
  pipeline->start();
  REQUIRE_FALSE(pipeline->is_running());
  REQUIRE_FALSE(pipeline->is_running());
  REQUIRE_FALSE(pipeline->is_running());
  std::this_thread::sleep_for(std::chrono::seconds(1));
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

TEST_CASE("spawned command should return true once again", "[pipeline]")
{
  std::cout << "Started\n";
  std::unique_ptr<Pipeline> pipeline =
      std::make_unique<Pipeline>(R"(F:\WorkFiles\http_server\build\processes\Debug\process_exit_properly.exe)");
  pipeline->start();
  REQUIRE(pipeline->is_running());
  REQUIRE(pipeline->is_running());
  REQUIRE(pipeline->is_running());
  std::this_thread::sleep_for(std::chrono::seconds(1));
  pipeline->stop();
  std::cout << "End\n";
}

TEST_CASE("jobs are equal", "[jobs]")
{
  Job job1("1");
  Job job2("1");
  REQUIRE(job1 == job2);
}

TEST_CASE("file", "[files]")
{
  Poco::File file(R"(D:\WorkFiles\http_server\static_html\favicon1.ico)");

  REQUIRE_THROWS_AS(file.isFile(), Poco::FileNotFoundException);
}

TEST_CASE("job serialize", "[jobs]")
{
  JobList job_list;
  Job job1;
  job_list.push_back(job1);
  Job job2;
  job_list.push_back(job2);

  std::stringstream ss; // any stream can be used
  {
    cereal::BinaryOutputArchive oarchive(ss); // Create an output archive
    // oarchive(CEREAL_NVP(job1));
    // oarchive(job1);
    oarchive << job_list;
  }
  std::cout << ss.str();
  JobList job_list2;
  ss.seekp(0);
  {
    cereal::BinaryInputArchive iarchive(ss);
    iarchive >> job_list2;
  }
  REQUIRE(job1 == job_list2[0]);
  std::stringstream ss1;
  {
    cereal::JSONOutputArchive oarchive_json(ss1);
    oarchive_json(CEREAL_NVP(job_list2));
  }
  std::cout << std::endl << ss1.str() << std::endl;
}

TEST_CASE("Media command serialize", "[media_command]")
{
  std::stringstream ss;
  {
    // cereal::JSONOutputArchive oarchive_json(ss);
    cereal::YAMLOutputArchive archive(ss);
    archive(JobToMediaCommandMapper::get_map());
  }
  std::cout << std::endl << ss.str() << std::endl;
  ss.seekp(0);
  JobMediaCommandMap map;
  {
    cereal::YAMLInputArchive iarchive(ss);
    iarchive >> map;
  }
}

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

TEST_CASE("function request response", "[rpc]")
{
  std::vector<uint8_t> buffer; // sendFrame ---> receiveFrame
  {
    Job job("channel_id1");
    std::cout << job << std::endl;

    FunctionRequestData req;
    req.func_name = "strat_pipeline";
    {
      std::stringstream ss;
      {
        cereal::BinaryOutputArchive oarchive(ss);
        oarchive << CEREAL_NVP(job);
      }
      std::string s = ss.str();
      std::copy(s.begin(), s.end(), std::back_inserter(req.args));
    }

    FunctionRequestOrResponseData function_request_or_response_data;
    function_request_or_response_data.request_or_response = FunctionRequestOrResponseData::REQUEST;
    {
      std::stringstream ss;
      {
        cereal::BinaryOutputArchive oarchive(ss);
        oarchive << CEREAL_NVP(req);
      }
      std::string s = ss.str();
      std::copy(s.begin(), s.end(), std::back_inserter(function_request_or_response_data.data));
    }

    {
      std::stringstream ss;
      {
        cereal::BinaryOutputArchive oarchive(ss);
        oarchive << CEREAL_NVP(function_request_or_response_data);
      }
      std::string s = ss.str();
      std::copy(s.begin(), s.end(), std::back_inserter(buffer));
    }

    std::cout << "Sent Buffer size:----------- " << buffer.size() << std::endl;
  }

  {

    std::cout << "Received Buffer size:----------- " << buffer.size() << std::endl;
    FunctionRequestOrResponseData function_request_or_response_data;
    {
      int n = buffer.size();
      std::stringstream ss;
      std::copy(buffer.begin(), buffer.begin() + n, std::ostream_iterator<uint8_t>(ss));
      {
        cereal::BinaryInputArchive iarchive(ss);
        iarchive >> function_request_or_response_data;
      }
    }

    FunctionRequestData req;
    {
      int n = function_request_or_response_data.data.size();
      std::stringstream ss;
      std::copy(function_request_or_response_data.data.begin(), function_request_or_response_data.data.begin() + n,
                std::ostream_iterator<uint8_t>(ss));
      {
        cereal::BinaryInputArchive iarchive(ss);
        iarchive >> req;
      }
    }
    Job job;
    {
      int n = req.args.size();
      std::stringstream ss;
      std::copy(req.args.begin(), req.args.begin() + n, std::ostream_iterator<uint8_t>(ss));
      {
        cereal::BinaryInputArchive iarchive(ss);
        iarchive >> job;
      }
    }

    std::cout << "received: " << job << std::endl;
  }

  // std::stringstream ss;
  // {
  //   cereal::BinaryOutputArchive oarchive(ss);
  //   FunctionRequestOrResponseData function_request_or_response_data;
  //   function_request_or_response_data.data = req.args;
  //   oarchive << function_request_or_response_data.data;
  // }
  // FunctionRequestData req_;
  // ss.seekp(0);
  // {
  //   cereal::BinaryInputArchive iarchive(ss);
  //   FunctionRequestOrResponseData function_request_or_response_data;
  //   iarchive >> function_request_or_response_data.data;
  //   req_.args = function_request_or_response_data.data;
  // }
  // for (auto &&i : req_.args)
  // {
  //   std::cout << i << " ";
  // }
  // std::cout << std::endl;

  // FunctionResponseData res;
  // res.args = {0, 1, 0};

  // FunctionRequestOrResponseData res_req;
  // res_req.function_request = 0;
  // res_req.data = res.args;
}
void foo(void) { std::cout << "foo" << std::endl; }

// template <typename R, typename... A> void test(R (*func)(A...))
// {
//   static_assert(!std::is_void<R>::value, "void return type is not allowed");
// }
void call_foo()
{
  auto f = foo;
  // test(f);
}
TEST_CASE("call decorator", "[rpc]") { call_foo(); }