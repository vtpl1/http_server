find_package(Poco REQUIRED Util Net)

add_subdirectory(controllers)
add_subdirectory(datamodels)
add_subdirectory(http_server_helper)

add_executable(entrypoint
    src/entry_point.cpp
)

target_link_libraries(entrypoint
    PRIVATE utils
    PRIVATE controllers
    PRIVATE datamodels
    PRIVATE Poco::Util
    PRIVATE Poco::Net
)

set_property(TARGET entrypoint PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")