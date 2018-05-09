cmake_minimum_required(VERSION 3.7.0)

include(GNUInstallDirs)

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wextra")
endif()

# Get git version info
execute_process(COMMAND
    "git"
    describe
    --tags
    --dirty
    WORKING_DIRECTORY
    "${CMAKE_CURRENT_SOURCE_DIR}"
    RESULT_VARIABLE
    GIT_RESULT
    OUTPUT_VARIABLE
    GIT_VERSION
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE)
if(NOT GIT_RESULT EQUAL 0)
    execute_process(COMMAND
        "git"
        describe
        --all
        WORKING_DIRECTORY
        "${CMAKE_CURRENT_SOURCE_DIR}"
        RESULT_VARIABLE
        GIT_RESULT
        OUTPUT_VARIABLE
        GIT_VERSION
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(NOT GIT_RESULT EQUAL 0)
        set(GIT_VERSION "INVALID")
    endif()
endif()
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/application/version.cpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/application/version.cpp
)
set(version_file "${CMAKE_CURRENT_BINARY_DIR}/application/version.cpp")

add_subdirectory(verdigris)
add_subdirectory(application)
