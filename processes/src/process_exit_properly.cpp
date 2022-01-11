// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include <Poco/Util/ServerApplication.h>

class EntryPoint : public Poco::Util::ServerApplication
{
  int main(const ArgVec& args) final { return Application::EXIT_OK; }
};

POCO_SERVER_MAIN(EntryPoint);