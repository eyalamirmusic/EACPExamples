set(CMAKE_FOLDER Apps)

if (APPLE OR WIN32)
    add_subdirectory(WebUIDemo)
    add_subdirectory(PhysicsDemo)
endif ()
