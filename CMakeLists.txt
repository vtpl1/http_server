
cmake_minimum_required(VERSION 3.16)
project(http_server)

set(CMAKE_CXX_CLANG_TIDY
    clang-tidy;)

find_package(Git)
if(Git_FOUND)
    execute_process(COMMAND ${GIT_EXECUTABLE} describe --always --abbrev=40
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE GIT_COMMIT_HASH)
    message(STATUS gch: ${GIT_COMMIT_HASH})
    add_definitions(-DGIT_COMMIT_HASH="${GIT_COMMIT_HASH}")

    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE GIT_COMMIT_BRANCH)
    message(STATUS gbr: ${GIT_COMMIT_BRANCH})
    add_definitions(-DGIT_COMMIT_BRANCH="${GIT_COMMIT_BRANCH}")
endif() #git

add_subdirectory(utils)
add_subdirectory(entrypoint)
# add_subdirectory(processes)
add_subdirectory(tests)