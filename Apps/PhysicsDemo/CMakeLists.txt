find_package(ReactPhysics3D REQUIRED)

# World.cpp is in COMMAND_SOURCES (not SOURCES) because Commands.cpp's
# `reset()` takes the address of the free function `resetWorld()` via
# MIRO_EXPORT_COMMAND, which forces the schema-export tool to resolve
# the body. Putting World.cpp in COMMAND_SOURCES gets it into both the
# executable and the schema tool. As a consequence rp3d also needs to
# link into the schema tool.
eacp_add_webview_app(PhysicsDemo
        SOURCES         Main.cpp
        COMMAND_SOURCES Commands.cpp World.cpp
        WEB_DIR         ${CMAKE_CURRENT_SOURCE_DIR}/web
        BUNDLE_ID       com.eacpexamples.physicsdemo
        BUNDLE_NAME     "Physics Demo")

target_link_libraries(PhysicsDemo PRIVATE reactphysics3d)
target_link_libraries(PhysicsDemoSchema PRIVATE reactphysics3d)
