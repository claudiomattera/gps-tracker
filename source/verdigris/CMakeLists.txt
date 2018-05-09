cmake_minimum_required(VERSION 3.7.0)

project(verdigris LANGUAGES CXX)

add_library(verdigris INTERFACE)

target_include_directories(
    verdigris
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:>
)
