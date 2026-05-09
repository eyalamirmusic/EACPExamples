#include "WorldApi.h"
#include <eacp/Core/Threads/Timer.h>
#include <eacp/WebView/WebView.h>

using namespace eacp;
using namespace Graphics;

struct PhysicsDemoApp
{
    PhysicsDemoApp()
    {
        setApplicationMenuBar(buildDefaultWebViewMenuBar());
        window.setContentView(webView);
        bridge.useStaticRegistry();
    }

    void tick()
    {
        stepWorld(1.0f / 60.0f);
        bridge.send("worldState", snapshotWorld());
    }

    WebView webView {embeddedOptions("WebApp")};
    WebViewBridge bridge {webView};
    Window window;
    Threads::Timer timer {[this] { tick(); }, 60};
};

int main()
{
    eacp::Apps::run<PhysicsDemoApp>();
    return 0;
}
