# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

find_package(Protobuf COMPONENTS libprotobuf protoc)
find_package(cereal REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS interfaces/job.proto)

add_library(protobuf_helper_datamodels STATIC
    src/protobuf_helper.cpp

    ${PROTO_SRCS}
)

target_precompile_headers(protobuf_helper_datamodels
    PUBLIC ${PROTO_HDRS}
    PUBLIC inc/protobuf_helper.h
)

target_include_directories(protobuf_helper_datamodels
    PUBLIC inc
    PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(protobuf_helper_datamodels
    PUBLIC protobuf::libprotobuf
)

set_target_properties(
    protobuf_helper_datamodels
    PROPERTIES
         CXX_CLANG_TIDY ""
)

add_library(datamodels STATIC
    src/job.cpp
    src/status.cpp
    src/media_command.cpp
    src/job_media_command_map.cpp
)

target_include_directories(datamodels
    PUBLIC inc
)

target_link_libraries(datamodels
    PUBLIC cereal::cereal
    PUBLIC protobuf_helper_datamodels
)

target_precompile_headers(datamodels
    PUBLIC inc/job.h
    PUBLIC inc/status.h
    PUBLIC inc/media_command.h
    PUBLIC inc/job_media_command_map.h
)
