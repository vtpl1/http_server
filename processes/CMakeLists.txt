find_package(Poco REQUIRED Util)

add_executable(process_exit_after
    src/process_exit_after.cpp
)

target_link_libraries(process_exit_after
    PRIVATE Poco::Util
)

add_executable(process_never_exit
  src/process_never_exit.cpp
)

target_link_libraries(process_never_exit
  PRIVATE Poco::Util
)

add_executable(process_exit_properly
  src/process_exit_properly.cpp
)

target_link_libraries(process_exit_properly
  PRIVATE Poco::Util
)
