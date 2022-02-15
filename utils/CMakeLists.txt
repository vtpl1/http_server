find_package(spdlog REQUIRED)

add_library(utils STATIC
  src/logging.cpp
  src/Chameleon.cpp
  src/ConfigFile.cpp
)

target_precompile_headers(utils
  PUBLIC inc/logging.h
  PUBLIC inc/Chameleon.h
  PUBLIC inc/ConfigFile.h
)


target_include_directories(utils
  PUBLIC inc
)

target_link_libraries(utils
  PUBLIC spdlog::spdlog
)

set_property(TARGET utils PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")