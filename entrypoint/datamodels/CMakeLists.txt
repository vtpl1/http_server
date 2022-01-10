# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

add_library(datamodels
    src/job.cpp
    inc/job.h
)

target_include_directories(datamodels
    PUBLIC inc
)