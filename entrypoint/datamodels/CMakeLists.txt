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

set_property(TARGET datamodels PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")