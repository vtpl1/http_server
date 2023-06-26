# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************
if(MSVC)
find_package(Poco REQUIRED Util NetSSLWin)
else()
find_package(Poco REQUIRED Util NetSSL)
endif()
find_package(Protobuf COMPONENTS libprotobuf protoc)
find_package(logutil REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS interfaces/status.proto)

add_library(protobuf_helper_resource_monitor STATIC
    src/protobuf_helper.cpp
    ${PROTO_SRCS}
)

target_precompile_headers(protobuf_helper_resource_monitor
  PUBLIC ${PROTO_HDRS}
  PUBLIC inc/protobuf_helper.h
)

target_include_directories(protobuf_helper_resource_monitor
    PUBLIC inc
    PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(protobuf_helper_resource_monitor
    PUBLIC protobuf::libprotobuf
)

set_target_properties(
    protobuf_helper_resource_monitor
    PROPERTIES
         CXX_CLANG_TIDY ""
)


add_library(resource_monitor STATIC
    src/monitor.cpp
)

target_precompile_headers(resource_monitor
  PUBLIC inc/monitor.h
)

target_include_directories(resource_monitor
    PUBLIC inc
)


target_link_libraries(resource_monitor
    PUBLIC Poco::Util
    PRIVATE logutil::core
    PUBLIC protobuf_helper_resource_monitor
)
if(MSVC)
target_link_libraries(resource_monitor
    PUBLIC Poco::NetSSLWin
)
else()
target_link_libraries(resource_monitor
    PUBLIC Poco::NetSSL
)
endif()

add_executable(resource_monitor_test
    src/monitor_test.cpp
)

target_link_libraries(resource_monitor_test
    PUBLIC resource_monitor
)

# set_property(TARGET protobuf_helper_resource_monitor PROPERTY
#     MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# set_property(TARGET resource_monitor PROPERTY
#     MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# set_property(TARGET resource_monitor_test PROPERTY
#     MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")