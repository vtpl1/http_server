// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <Poco/Util/ServerApplication.h>
#include <iostream>
#include <memory>
#include <thread>
class Tester
{
private:
  std::unique_ptr<std::thread> _thread;

public:
  Tester()
  {
    _thread = std::make_unique<std::thread>(&Tester::run, this);
    _thread->detach();
  }
  ~Tester() {}
  void run()
  {
    std::cout << "Thread started" << std::endl;
    while (true) {
      std::cout << "Thread alive" << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << "Thread stopped" << std::endl;
  }
};

class EntryPoint : public Poco::Util::ServerApplication
{

  int main(const ArgVec& args) final
  {

    Tester* tester = new Tester();
    std::cout << "Process started" << std::endl;
    waitForTerminationRequest();
    std::cout << "Process stopped" << std::endl;
    return Application::EXIT_OK;
  }
};

POCO_SERVER_MAIN(EntryPoint);