find_package(Poco REQUIRED Util)
find_package(logutil REQUIRED)

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
    PRIVATE logutil::core
    PRIVATE controllers
    PRIVATE datamodels
    PRIVATE Poco::Util
)
