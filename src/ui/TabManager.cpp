#include "TabManager.h"
#include "../core/LumenClient.h"

#include "include/cef_browser.h"
#include "include/cef_browser_host.h"

TabManager::TabManager(HWND host_hwnd) : host_hwnd_(host_hwnd) {}

void TabManager::OpenTab(const CefString& url) {
    CefWindowInfo wi;
    RECT rc{ 0, 52, 1280, 800 }; // placeholder; BrowserView will resize
    wi.SetAsChild(host_hwnd_, rc);

    CefRefPtr<LumenClient> client = new LumenClient(this);
    CefBrowserSettings bs;

    Tab tab;
    tab.id  = NextId();
    tab.url = url.ToString();
    tabs_.push_back(tab);
    active_id_ = tab.id;

    // Async creation; OnAfterCreated fills tab.browser
    CefBrowserHost::CreateBrowser(wi, client, url, bs, nullptr, nullptr);
}

void TabManager::CloseCurrentTab() {
    Tab* tab = ActiveTab();
    if (tab && tab->browser)
        tab->browser->GetHost()->CloseBrowser(false);
}

void TabManager::SwitchTo(int tab_id) {
    Tab* prev = ActiveTab();
    if (prev && prev->browser)
        ShowWindow(prev->browser->GetHost()->GetWindowHandle(), SW_HIDE);

    active_id_ = tab_id;

    Tab* next = ActiveTab();
    if (next && next->browser)
        ShowWindow(next->browser->GetHost()->GetWindowHandle(), SW_SHOW);
}

void TabManager::OnBrowserCreated(CefRefPtr<CefBrowser> browser) {
    // Match to pending tab (last pushed without browser set)
    for (auto& tab : tabs_) {
        if (!tab.browser) {
            tab.browser = browser;
            return;
        }
    }
    // Fallback: add new tab entry
    Tab tab;
    tab.id      = NextId();
    tab.browser = browser;
    tabs_.push_back(tab);
    active_id_ = tab.id;
}

void TabManager::OnBrowserClosed(CefRefPtr<CefBrowser> browser) {
    auto it = std::find_if(tabs_.begin(), tabs_.end(),
        [&](const Tab& t){ return t.browser && t.browser->IsSame(browser); });
    if (it == tabs_.end()) return;

    bool was_active = (it->id == active_id_);
    tabs_.erase(it);

    if (was_active && !tabs_.empty())
        SwitchTo(tabs_.back().id);
}

Tab* TabManager::ActiveTab() {
    for (auto& tab : tabs_)
        if (tab.id == active_id_) return &tab;
    return nullptr;
}
