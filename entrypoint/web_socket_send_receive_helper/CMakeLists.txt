# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

if(MSVC)
find_package(Poco REQUIRED Foundation NetSSLWin)
else()
find_package(Poco REQUIRED Foundation NetSSL)
endif()

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
    PRIVATE utils
    # PUBLIC rpc_handler
    PUBLIC Poco::Foundation
    PUBLIC Poco::NetSSLWin
)
if(MSVC)
target_link_libraries(web_socket_send_receive_helper
    PUBLIC Poco::NetSSLWin
)
else()
target_link_libraries(web_socket_send_receive_helper
    PUBLIC Poco::NetSSL
)
endif()

set_property(TARGET web_socket_send_receive_helper PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")