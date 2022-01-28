# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

find_package(Boost REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Poco REQUIRED Util)
add_library(http_server_async_helper
    src/http_server.cpp
    src/http_session.cpp
    src/server.cpp
    src/websocket_session.cpp
    # src/connection_manager.cpp

    inc/http_server.h


    inc/server.hpp
    inc/http_session.hpp
    inc/websocket_session.hpp
    # inc/connection_manager.hpp
)

target_precompile_headers(http_server_async_helper
  PUBLIC    <boost/asio.hpp>
  PRIVATE   <boost/beast.hpp>
)
if (MSVC)
target_compile_options(http_server_async_helper
    PUBLIC -bigobj
)
endif()
target_compile_definitions(http_server_async_helper
    PUBLIC WIN32_LEAN_AND_MEAN
    PUBLIC _WIN32_WINNT=_WIN32_WINNT_WIN7
)
target_include_directories(http_server_async_helper
    PUBLIC inc
)

target_link_libraries(http_server_async_helper
    PRIVATE utils
    PUBLIC rpc_handler
    PRIVATE Boost::boost
    PRIVATE Poco::Util
    PRIVATE OpenSSL::SSL
    PRIVATE OpenSSL::Crypto
)

set_property(TARGET http_server_async_helper PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")