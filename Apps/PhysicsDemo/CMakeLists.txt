find_package(ReactPhysics3D REQUIRED)

# World.cpp is in COMMAND_SOURCES (not SOURCES) because Commands.cpp's
# `reset()` takes the address of the free function `resetWorld()` via
# MIRO_EXPORT_COMMAND, which forces the schema-export tool to resolve
# the body. COMMAND_SOURCES land in the schema's underlying OBJECT
# library, which is linked into both the executable and the schema-
# export tool. rp3d is linked there with PUBLIC so its headers reach
# the OBJECT compile and its symbols propagate to both consumers.
eacp_add_webview_app(PhysicsDemo
        SOURCES         Main.cpp
        COMMAND_SOURCES Commands.cpp World.cpp PhysicsThread.cpp
        WEB_DIR         ${CMAKE_CURRENT_SOURCE_DIR}/web
        BUNDLE_ID       com.eacpexamples.physicsdemo
        BUNDLE_NAME     "Physics Demo")

target_link_libraries(_PhysicsDemoSchema_objects PUBLIC reactphysics3d)

# eacp_target_uses_schema(... HANDLERS) routes through the INTERFACE
# wrapper, but CMake 4.x does not propagate an OBJECT library's .o files
# through a transitive INTERFACE link — so WoPORT_COMMAND registrations
# from Commands.cpp would never be linked inrld.cpp's free functions
## (referenced from Main.cpp) and the MIRO_EX. Link the underscored
# OBJECT lib directly to get the objects in.
target_link_libraries(PhysicsDemo PRIVATE _PhysicsDemoSchema_objects)
