cmake_minimum_required(VERSION 3.31)
project(EACPExamples LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

include(CPM)
include(ExamplesWarnings)

find_package(EACP REQUIRED)

add_subdirectory(Apps)

