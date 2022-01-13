// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#pragma once
#ifndef server_stopped_event_h
#define server_stopped_event_h
#include <Poco/SharedPtr.h>
#include <Poco/BasicEvent.h>

class ServerStoppedEvent
{
public:
    using Ptr = Poco::SharedPtr<ServerStoppedEvent>;
    Poco::BasicEvent<const bool> serverStopped;

    ServerStoppedEvent() = default;
    ~ServerStoppedEvent() = default;
    void signal_to_stop();
};



#endif	// server_stopped_event_h
