# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

add_library(rpc_handler
    src/rpc_manager.cpp

    src/function_request_data.cpp
    src/function_response_data.cpp
    src/function_request_or_response_data.cpp

    inc/rpc_manager.h

    inc/function_request_data.h
    inc/function_response_data.h
    inc/function_request_or_response_data.h
)

target_include_directories(rpc_handler
    PUBLIC inc
)

target_link_libraries(rpc_handler
    PRIVATE utils
    PRIVATE Poco::Foundation
)

set_property(TARGET rpc_handler PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")