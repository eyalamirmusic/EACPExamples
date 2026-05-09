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
