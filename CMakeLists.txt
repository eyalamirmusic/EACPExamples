cmake_minimum_required(VERSION 3.31)
project(EACPExamples LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (APPLE)
    enable_language(OBJCXX)

    # rp3d ships a Profiler.cpp that compiles to an empty object when
    # RP3D_PROFILING_ENABLED=OFF (our default). Apple's ar+ranlib
    # pipeline emits a benign "has no symbols" advisory for every
    # empty TU and -no_warning_for_no_symbols isn't a ranlib flag
    # (it's silently ignored). Switch the static-archive command to
    # `libtool -static`, which is Apple's modern static archiver and
    # does honor that flag.
    find_program(APPLE_LIBTOOL libtool REQUIRED)
    set(_apple_libtool_static
            "${APPLE_LIBTOOL} -static -no_warning_for_no_symbols -o <TARGET> <OBJECTS>")
    set(CMAKE_C_CREATE_STATIC_LIBRARY "${_apple_libtool_static}")
    set(CMAKE_CXX_CREATE_STATIC_LIBRARY "${_apple_libtool_static}")
endif ()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

include(CPM)
include(ExamplesWarnings)

set(EACP_WEBVIEW_VITE_BUILD ON CACHE BOOL "Drive the Vite build from CMake")

find_package(EACP REQUIRED)

if (PROJECT_IS_TOP_LEVEL)
    add_subdirectory(Apps)
endif ()
