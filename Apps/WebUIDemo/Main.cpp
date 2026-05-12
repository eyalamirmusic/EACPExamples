#include <eacp/WebView/WebView.h>

using namespace eacp;
using namespace Graphics;

struct WebUIDemoApp
{
    WebUIDemoApp()
    {
        setApplicationMenuBar(buildDefaultWebViewMenuBar());
        window.setContentView(webView);

        transport.getBridge().useStaticRegistry();
    }

    WebView webView {embeddedOptions("WebApp")};
    WebViewBridge transport {webView};
    Window window;
};

int main()
{
    eacp::Apps::run<WebUIDemoApp>();
    return 0;
}
