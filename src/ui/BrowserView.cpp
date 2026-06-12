#include "BrowserView.h"
#include "TabManager.h"
#include "../core/LumenClient.h"

#include "include/cef_browser.h"
#include "include/cef_browser_host.h"
#include "include/wrapper/cef_helpers.h"
#include "include/wrapper/cef_message_router.h"

BrowserView::BrowserView(HWND parent, TabManager* tab_manager)
    : parent_(parent), tab_manager_(tab_manager) {
    client_ = new LumenClient(tab_manager);

    // Wire up the message router for lumenQuery IPC
    CefMessageRouterConfig router_config;
    router_config.js_query_function = "lumenQuery";
    router_config.js_cancel_function = "lumenQueryCancel";
    auto router = CefMessageRouterBrowserSide::Create(router_config);
    client_->SetMessageRouter(router);
}

BrowserView::~BrowserView() {
    if (browser_)
        browser_->GetHost()->CloseBrowser(true);
}

void BrowserView::Create(int x, int y, int w, int h) {
    CefWindowInfo wi;
    RECT rc{ x, y, x + w, y + h };
    wi.SetAsChild(parent_, rc);

    CefBrowserSettings bs;
    bs.javascript = STATE_ENABLED;
    bs.javascript_close_windows = STATE_DISABLED;

    browser_ = CefBrowserHost::CreateBrowserSync(
        wi, client_, "lumen://newtab", bs, nullptr, nullptr);
}

void BrowserView::SetBounds(int x, int y, int w, int h) {
    if (!browser_) return;
    HWND hwnd = browser_->GetHost()->GetWindowHandle();
    if (hwnd)
        SetWindowPos(hwnd, nullptr, x, y, w, h, SWP_NOZORDER);
}
