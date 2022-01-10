# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

add_library(controllers
    src/job_list_manager.cpp
    src/pipeline_manager.cpp
    src/pipeline.cpp

    inc/job_list_manager.h
    inc/pipeline_manager.h
    inc/pipeline.h
)

target_include_directories(controllers
    PUBLIC inc
)

target_link_libraries(controllers
    PRIVATE utils
    PRIVATE datamodels
)