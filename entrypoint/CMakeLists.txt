find_package(Poco REQUIRED Util Net)

add_subdirectory(datamodels)
add_subdirectory(rpc_handler)
add_subdirectory(web_socket_send_receive_helper)
add_subdirectory(http_server_helper)
add_subdirectory(http_server_async_helper)
add_subdirectory(controllers)

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