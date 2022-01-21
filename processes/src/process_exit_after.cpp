// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <Poco/Util/ServerApplication.h>
#include <thread>
#include <iostream>

constexpr int time_out = 10;
class EntryPoint : public Poco::Util::ServerApplication
{
  int main(const ArgVec& args) final
  {
    std::cout << "Process started" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(time_out));
    std::cout << "Process stopped" << std::endl;
    return Application::EXIT_OK;
  }
};

POCO_SERVER_MAIN(EntryPoint);