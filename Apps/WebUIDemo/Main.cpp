#include <eacp/WebView/WebView.h>

using namespace eacp;
using namespace Graphics;

struct WebUIDemoApp
{
    WebUIDemoApp()
    {
        setApplicationMenuBar(buildDefaultWebViewMenuBar());
        window.setContentView(webView);

        bridge.useStaticRegistry();
    }

    WebView webView {embeddedOptions("WebApp")};
    WebViewBridge bridge {webView};
    Window window;
};

int main()
{
    eacp::Apps::run<WebUIDemoApp>();
    return 0;
}
