#include "ChromeOverlay.h"
#include "TabManager.h"
#include "../core/LumenClient.h"

#include "include/cef_browser.h"
#include "include/cef_browser_host.h"

ChromeOverlay::ChromeOverlay(HWND parent, TabManager* tab_manager)
    : parent_(parent), tab_manager_(tab_manager) {
    // Overlay uses its own LumenClient; no tab management needed here
    client_ = new LumenClient(nullptr);
}

ChromeOverlay::~ChromeOverlay() {
    if (browser_)
        browser_->GetHost()->CloseBrowser(true);
}

void ChromeOverlay::Create(int x, int y, int w, int h) {
    CefWindowInfo wi;
    RECT rc{ x, y, x + w, y + h };
    wi.SetAsChild(parent_, rc);

    CefBrowserSettings bs;
    bs.javascript = STATE_ENABLED;
    // Transparent background so the toolbar HTML controls its own bg
    bs.background_color = 0x00000000;

    browser_ = CefBrowserHost::CreateBrowserSync(
        wi, client_, "lumen://toolbar", bs, nullptr, nullptr);
}

void ChromeOverlay::SetBounds(int x, int y, int w, int h) {
    if (!browser_) return;
    HWND hwnd = browser_->GetHost()->GetWindowHandle();
    if (hwnd)
        SetWindowPos(hwnd, nullptr, x, y, w, h, SWP_NOZORDER);
}
