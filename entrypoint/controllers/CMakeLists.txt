# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

find_package(Poco REQUIRED Foundation)
find_package(yaml-cpp REQUIRED)

add_library(controllers STATIC
    src/end_point_manager.cpp
    src/job_list_manager.cpp
    src/pipeline_manager.cpp
    src/pipeline.cpp
    src/command_receiver.cpp
    src/job_to_media_command_mapper.cpp
)

target_precompile_headers(controllers
    PUBLIC inc/end_point_manager.h
    PUBLIC inc/job_list_manager.h
    PUBLIC inc/pipeline_manager.h
    PUBLIC inc/pipeline.h
    PUBLIC inc/command_receiver.h
    PUBLIC inc/job_to_media_command_mapper.h
)

target_include_directories(controllers
    PUBLIC inc
)

target_link_libraries(controllers
    PRIVATE utils
    PUBLIC datamodels
    PUBLIC http_server_helper
    PUBLIC web_socket_send_receive_helper
    PUBLIC rpc_handler
    PRIVATE Poco::Foundation
    PRIVATE yaml-cpp
)

set_property(TARGET controllers PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")