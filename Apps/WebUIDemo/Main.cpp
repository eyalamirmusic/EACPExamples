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

    Miro::Bridge bridge;
    WebView webView {embeddedOptions("WebApp")};
    WebViewBridge transport {webView, bridge};
    Window window;
};

int main()
{
    eacp::Apps::run<WebUIDemoApp>();
    return 0;
}
