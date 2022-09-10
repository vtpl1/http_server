# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

find_package(logutil REQUIRED)
if (MSVC)
find_package(Poco REQUIRED Foundation Util NetSSLWin)
else()
find_package(Poco REQUIRED Foundation Util NetSSL)
endif()


add_library(http_server_helper STATIC
    src/http_server.cpp
    src/generic_http_request_handler_factory.cpp
    src/options_request_handler.cpp
    src/method_not_supported_request_handler.cpp
    src/not_found_request_handler.cpp
    src/file_request_handler.cpp
    src/poco_net_stoppable_http_request_handler.cpp
    src/server_stopped_event.cpp
    src/web_socket_request_handler.cpp
    src/web_socket_page_request_handler.cpp
)

target_precompile_headers(http_server_helper
    PUBLIC inc/http_server.h
    PUBLIC inc/generic_http_request_handler_factory.h
    PUBLIC inc/options_request_handler.h
    PUBLIC inc/method_not_supported_request_handler.h
    PUBLIC inc/not_found_request_handler.h
    PUBLIC inc/file_request_handler.h
    PUBLIC inc/poco_net_stoppable_http_request_handler.h
    PUBLIC inc/server_stopped_event.h
    PUBLIC inc/web_socket_request_handler.h
    PUBLIC inc/web_socket_page_request_handler.h
)

target_include_directories(http_server_helper
    PUBLIC inc
)

target_link_libraries(http_server_helper
    PRIVATE logutil::logutil
    PUBLIC rpc_handler
    PUBLIC web_socket_send_receive_helper
    PUBLIC Poco::Foundation
    PUBLIC Poco::Util
)
if (MSVC)
target_link_libraries(http_server_helper
    PUBLIC Poco::NetSSLWin
)
else()
target_link_libraries(http_server_helper
    PUBLIC Poco::NetSSL
)
endif()
