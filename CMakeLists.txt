cmake_minimum_required(VERSION 3.31)
project(EACPExamples LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (APPLE)
    enable_language(OBJCXX)
endif ()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

include(CPM)
include(ExamplesWarnings)

set(EACP_WEBVIEW_VITE_BUILD ON CACHE BOOL "Drive the Vite build from CMake")

find_package(EACP REQUIRED)
find_package(Miro REQUIRED)

if (PROJECT_IS_TOP_LEVEL)
    add_subdirectory(Apps)
endif ()
