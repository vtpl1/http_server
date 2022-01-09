
cmake_minimum_required(VERSION 3.16)
project(http_server)
message(STATUS "VCPKG_INSTALLED_DIR " ${VCPKG_INSTALLED_DIR})

set(CMAKE_CXX_CLANG_TIDY
    clang-tidy;)

add_subdirectory(utils)
add_subdirectory(entrypoint)