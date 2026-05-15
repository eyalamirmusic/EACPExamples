include(CPM)

CPMAddPackage(
        NAME reactphysics3d
        GITHUB_REPOSITORY DanielChappuis/reactphysics3d
        GIT_TAG v0.10.2
        SYSTEM YES
        OPTIONS
            "RP3D_COMPILE_TESTBED OFF"
            "RP3D_COMPILE_TESTS OFF"
            "RP3D_GENERATE_DOCUMENTATION OFF")

# Silence warnings inside rp3d's own .cpp files. SYSTEM YES above
# suppresses warnings from its headers when consumers include them,
# but compiling its sources still picks up our project's warning flags
# (or its own), and rp3d trips a few -Wreturn-type / -Wnontrivial-
# memcall on recent clang. Treat the whole library as third-party.
if (TARGET reactphysics3d)
    if (MSVC)
        target_compile_options(reactphysics3d PRIVATE /w)
    else ()
        target_compile_options(reactphysics3d PRIVATE -w)
    endif ()
endif ()

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