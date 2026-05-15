find_package(ReactPhysics3D REQUIRED)

eacp_add_webview_app(PhysicsDemo
        SOURCES         Main.cpp
        COMMAND_SOURCES Commands.cpp World.cpp PhysicsThread.cpp
        WEB_DIR         ${CMAKE_CURRENT_SOURCE_DIR}/web
        BUNDLE_ID       com.eacpexamples.physicsdemo
        BUNDLE_NAME     "Physics Demo")

target_link_libraries(PhysicsDemo PRIVATE reactphysics3d)
target_link_libraries(PhysicsDemoSchema_Codegen PRIVATE reactphysics3d)
