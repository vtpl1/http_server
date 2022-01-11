// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <Poco/Util/ServerApplication.h>
#include <iostream>

class EntryPoint : public Poco::Util::ServerApplication
{
  int main(const ArgVec& args) final {
    std::cout << "Process started" << std::endl;
    waitForTerminationRequest();
    std::cout << "Process stopped" << std::endl;
    return Application::EXIT_OK;
  }
};

POCO_SERVER_MAIN(EntryPoint);