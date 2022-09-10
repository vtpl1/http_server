# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

find_package(cereal REQUIRED)
find_package(logutil REQUIRED)
add_library(rpc_handler STATIC
    src/rpc_manager.cpp
    src/function_request_data.cpp
    src/function_response_data.cpp
    src/function_request_or_response_data.cpp
)

target_precompile_headers(rpc_handler
    PUBLIC inc/rpc_manager.h
    PUBLIC inc/function_request_data.h
    PUBLIC inc/function_response_data.h
    PUBLIC inc/function_request_or_response_data.h
)

target_include_directories(rpc_handler
    PUBLIC inc
)

target_link_libraries(rpc_handler
    PRIVATE logutil::logutil
    PUBLIC cereal::cereal
)
