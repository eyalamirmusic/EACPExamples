eacp_add_webview_app(WebUIDemo
        SOURCES         Main.cpp
        COMMAND_SOURCES Commands.cpp
        WEB_DIR         ${CMAKE_CURRENT_SOURCE_DIR}/web
        BUNDLE_ID       com.eacpexamples.webuidemo
        BUNDLE_NAME     "WebUI Demo")
