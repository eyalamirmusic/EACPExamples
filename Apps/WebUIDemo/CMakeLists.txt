eacp_add_webview_app(WebUIDemo
        SOURCES         Main.cpp
        COMMAND_SOURCES Commands.cpp
        WEB_DIR         ${CMAKE_CURRENT_SOURCE_DIR}/web
        BUNDLE_ID       com.eacpexamples.webuidemo
        BUNDLE_NAME     "WebUI Demo")

# eacp_target_uses_schema(... HANDLERS) routes through the INTERFACE
# wrapper, but CMake 4.x does not propagate an OBJECT library's .o files
# through a transitive INTERFACE link — so the MIRO_EXPORT_COMMAND
# static initializers from Commands.cpp would silently never register.
# Link the underscored OBJECT lib directly to get the objects in.
target_link_libraries(WebUIDemo PRIVATE _WebUIDemoSchema_objects)
