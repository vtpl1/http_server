# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

find_package(Poco REQUIRED Foundation)

add_library(controllers
    src/end_point_manager.cpp
    src/job_list_manager.cpp
    src/pipeline_manager.cpp
    src/pipeline.cpp

    inc/end_point_manager.h
    inc/job_list_manager.h
    inc/pipeline_manager.h
    inc/pipeline.h
)

target_include_directories(controllers
    PUBLIC inc
)

target_link_libraries(controllers
    PRIVATE utils
    PUBLIC datamodels
    PUBLIC httplib
    PRIVATE Poco::Foundation
)

set_property(TARGET controllers PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")