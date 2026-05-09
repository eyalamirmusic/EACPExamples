#include "WorldApi.h"
#include <eacp/Core/Threads/Timer.h>
#include <eacp/WebView/WebView.h>
#include <ea_data_structures/ea_data_structures.h>

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
        bridge.send("worldTick", snapshotTickFromWorld());
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
