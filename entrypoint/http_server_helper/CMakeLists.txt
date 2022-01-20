# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

find_package(Poco REQUIRED Foundation Net)

add_library(http_server_helper
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
    src/rpc_manager.cpp

    src/function_request_data.cpp
    src/function_response_data.cpp
    src/function_request_or_response_data.cpp

    inc/http_server.h
    inc/generic_http_request_handler_factory.h
    inc/options_request_handler.h
    inc/method_not_supported_request_handler.h
    inc/not_found_request_handler.h
    inc/file_request_handler.h
    inc/poco_net_stoppable_http_request_handler.h
    inc/server_stopped_event.h
    inc/web_socket_request_handler.h
    inc/web_socket_page_request_handler.h
    inc/rpc_manager.h
    inc/function_request_data.h
    inc/function_response_data.h
    inc/function_request_or_response_data.h
)

target_include_directories(http_server_helper
    PUBLIC inc
)

target_link_libraries(http_server_helper
    PRIVATE utils
    PRIVATE Poco::Foundation
    PRIVATE Poco::Net
)

set_property(TARGET http_server_helper PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")