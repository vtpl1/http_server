// *****************************************************
//    Copyright 2022 Videonetics Technology Pvt Ltd
// *****************************************************

#include "server_stopped_event.h"

void ServerStoppedEvent::signal_to_stop()
{
    serverStopped(this, true);
}