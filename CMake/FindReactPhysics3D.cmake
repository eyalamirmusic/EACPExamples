include(CPM)

CPMAddPackage(
        NAME reactphysics3d
        GITHUB_REPOSITORY DanielChappuis/reactphysics3d
        GIT_TAG v0.10.2
        OPTIONS
            "RP3D_COMPILE_TESTBED OFF"
            "RP3D_COMPILE_TESTS OFF"
            "RP3D_GENERATE_DOCUMENTATION OFF")
