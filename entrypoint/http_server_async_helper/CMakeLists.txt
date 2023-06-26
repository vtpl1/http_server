# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

find_package(Boost REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Poco REQUIRED Util)
find_package(logutil REQUIRED)

add_library(http_server_async_helper STATIC
    src/http_server.cpp
    src/http_session.cpp
    src/server.cpp
    src/websocket_session.cpp
    # src/connection_manager.cpp

    # inc/connection_manager.hpp
)

target_precompile_headers(http_server_async_helper
    PUBLIC inc/http_server.h
    PUBLIC inc/server.hpp
    PUBLIC inc/http_session.hpp
    PUBLIC inc/websocket_session.hpp
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
    PRIVATE logutil::core
    PUBLIC rpc_handler
    PRIVATE Boost::boost
    PRIVATE Poco::Util
    PRIVATE OpenSSL::SSL
    PRIVATE OpenSSL::Crypto
)
