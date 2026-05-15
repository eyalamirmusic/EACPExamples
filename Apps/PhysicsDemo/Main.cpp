#include "WorldApi.h"
#include <eacp/WebView/WebView.h>

using namespace eacp;
using namespace Graphics;

struct PhysicsDemoApp
{
    PhysicsDemoApp()
    {
        setApplicationMenuBar(buildDefaultWebViewMenuBar());
        window.setContentView(webView);
    }

    void tick()
    {
        stepWorld(1.0f / 60.0f);
        transport.getBridge().emit("worldTick", snapshotTickFromWorld());
    }

    WebView webView {embeddedOptions("WebApp")};
    WebViewBridge transport {webView};
    Window window;
    Threads::Timer timer {[this] { tick(); }, 60};
};

int main()
{
    eacp::Apps::run<PhysicsDemoApp>();
    return 0;
}
