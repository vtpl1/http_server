find_package(Poco REQUIRED Util Net)

add_subdirectory(controllers)
add_subdirectory(datamodels)

add_executable(http_server
    src/EntryPoint.cpp
)

target_link_libraries(http_server
    PRIVATE utils
    PRIVATE controllers
    PRIVATE datamodels
    PRIVATE httplib
    PRIVATE Poco::Util
    PRIVATE Poco::Net
)

set_property(TARGET http_server PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")