cmake_minimum_required(VERSION 3.16)
project(pocowebsocket)

find_package(Poco REQUIRED Util Net)


add_executable(pocowebsocketserver
  src/server_socket.cpp
)

add_executable(pocowebsocketclient
  src/client_socket.cpp
)

target_link_libraries(pocowebsocketserver
  PRIVATE Poco::Util
  PRIVATE Poco::Net
)

target_link_libraries(pocowebsocketclient
  PRIVATE Poco::Util
  PRIVATE Poco::Net
)

set_property(TARGET pocowebsocketserver PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set_property(TARGET pocowebsocketclient PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")