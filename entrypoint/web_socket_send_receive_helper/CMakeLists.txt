# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

find_package(logutil REQUIRED)
find_package(Poco REQUIRED Foundation Net)

add_library(web_socket_send_receive_helper STATIC
    src/web_socket_send_receive_helper.cpp
)

target_precompile_headers(web_socket_send_receive_helper
    PUBLIC inc/web_socket_send_receive_helper.h
)

target_include_directories(web_socket_send_receive_helper
    PUBLIC inc
)

target_link_libraries(web_socket_send_receive_helper
    PRIVATE logutil::core
    # PUBLIC rpc_handler
    PUBLIC Poco::Foundation
    PUBLIC Poco::Net
)
