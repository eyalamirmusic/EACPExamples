add_executable(WebUIDemo Main.cpp Commands.cpp)
target_link_libraries(WebUIDemo PRIVATE
        eacp-webview
        eacp-network-rpc
        eacp_examples_warnings)

miro_add_type_export(
        NAME WebUIDemoSchema
        SOURCES Commands.cpp
        OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/web/src/generated
        OUTPUT_NAME schema
        FORMATS ts backend ts-server)

eacp_webview_generate_backend(
        OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/web/src/generated
        BASENAME schema)

add_dependencies(WebUIDemo WebUIDemoSchema)

eacp_webview_add_vite(WebUIDemo
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/web
        NAMESPACE WebResources
        CATEGORY WebApp)

set(BUNDLE_ID "com.eacpexamples.webuidemo")

set_target_properties(WebUIDemo PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "WebUI Demo"
        MACOSX_BUNDLE_GUI_IDENTIFIER ${BUNDLE_ID}
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER ${BUNDLE_ID})

set_default_target_setting(WebUIDemo)
