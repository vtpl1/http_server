# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

find_package(Poco REQUIRED Foundation)
find_package(yaml-cpp REQUIRED)

add_library(controllers
    src/end_point_manager.cpp
    src/job_list_manager.cpp
    src/pipeline_manager.cpp
    src/pipeline.cpp
    src/command_receiver.cpp
    src/job_to_media_command_mapper.cpp
    src/rpc_manager.cpp

    inc/end_point_manager.h
    inc/job_list_manager.h
    inc/pipeline_manager.h
    inc/pipeline.h
    inc/command_receiver.h
    inc/job_to_media_command_mapper.h
    inc/rpc_manager.h
)

target_include_directories(controllers
    PUBLIC inc
)

target_link_libraries(controllers
    PRIVATE utils
    PUBLIC datamodels
    PUBLIC http_server_helper
    PRIVATE Poco::Foundation
    PRIVATE yaml-cpp
)

set_property(TARGET controllers PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")