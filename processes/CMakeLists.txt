find_package(Poco REQUIRED Util)

add_executable(process_exit_after
    src/process_exit_after.cpp
)

target_link_libraries(process_exit_after
    PRIVATE Poco::Util
)

set_property(TARGET process_exit_after PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_executable(process_never_exit
  src/process_never_exit.cpp
)

target_link_libraries(process_never_exit
  PRIVATE Poco::Util
)

set_property(TARGET process_never_exit PROPERTY
MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_executable(process_exit_properly
  src/process_exit_properly.cpp
)

target_link_libraries(process_exit_properly
  PRIVATE Poco::Util
)

set_property(TARGET process_exit_properly PROPERTY
MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")