
cmake_minimum_required(VERSION 3.20)
project(http_server)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_definitions(-DPOCO_STATIC)

# if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
# set(CMAKE_CXX_CLANG_TIDY
#     clang-tidy;)
# set(CMAKE_C_CLANG_TIDY
#     clang-tidy;)
# endif()

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

    # the date of the commit
    execute_process(COMMAND ${GIT_EXECUTABLE} log -1 --format=%ad --date=local
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET OUTPUT_VARIABLE GIT_COMMIT_DATE)
    add_definitions(-DGIT_COMMIT_DATE="${GIT_COMMIT_DATE}")

    set(GIT_DETAILS "${GIT_COMMIT_BRANCH}_${GIT_COMMIT_HASH}_[${GIT_COMMIT_DATE}]")

    add_definitions(-DGIT_DETAILS="${GIT_DETAILS}")
endif() #git

add_subdirectory(resource_monitor)
add_subdirectory(entrypoint)
add_subdirectory(processes)
add_subdirectory(tests)