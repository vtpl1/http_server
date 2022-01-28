cmake_minimum_required(VERSION 3.17)
find_package(spdlog REQUIRED)

add_library(utils STATIC
  src/logging.cpp
  inc/logging.h
)

set_target_properties(utils
  PROPERTIES
    CXX_CLANG_TIDY ""
)

target_include_directories(utils
  PUBLIC inc
)

target_link_libraries(utils
  PUBLIC spdlog::spdlog
)

target_precompile_headers(utils
  PUBLIC    inc/logging.h
)

set_property(TARGET utils PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")