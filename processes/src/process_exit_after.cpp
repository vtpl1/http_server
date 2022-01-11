// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <Poco/Util/ServerApplication.h>
#include <thread>

class EntryPoint : public Poco::Util::ServerApplication
{
  int main(const ArgVec& args) final
  {
    std::cout << "Process started" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(10));
    std::cout << "Process stopped" << std::endl;
    return Application::EXIT_OK;
  }
};

POCO_SERVER_MAIN(EntryPoint);