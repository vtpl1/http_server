# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************

# find_package(protobuf COMPONENTS libprotobuf protoc)
find_package(cereal REQUIRED)

# protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
#     interfaces/status.proto
#     interfaces/job.proto
# )

# protobuf_generate(
#     LANGUAGE cpp
#     TARGET datamodels
#     PROTOS interfaces/status.proto
# )

add_library(datamodels
    src/job.cpp
    src/status.cpp
    src/media_command.cpp
    src/job_media_command_map.cpp

    inc/job.h
    inc/status.h
    inc/media_command.h
    inc/job_media_command_map.h

    # ${PROTO_SRCS}
    # ${PROTO_HDRS}
)

target_include_directories(datamodels
    PUBLIC inc
    PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
)

# target_link_libraries(datamodels
#     PUBLIC protobuf::libprotobuf
# )

target_link_libraries(datamodels
    PUBLIC cereal
)

# target_precompile_headers(datamodels
#     PUBLIC inc/job.h
#     PUBLIC inc/status.h
#     PUBLIC inc/media_command.h
#     PUBLIC inc/job_media_command_map.h
# )

set_property(TARGET datamodels PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")