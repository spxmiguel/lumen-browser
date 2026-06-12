#pragma once

#include "Tab.h"
#include "include/cef_browser.h"

#include <windows.h>
#include <vector>
#include <string>

class TabManager {
public:
    explicit TabManager(HWND host_hwnd);

    void OpenTab(const CefString& url);
    void CloseCurrentTab();
    void SwitchTo(int tab_id);

    void OnBrowserCreated(CefRefPtr<CefBrowser> browser);
    void OnBrowserClosed(CefRefPtr<CefBrowser> browser);

    bool IsEmpty() const { return tabs_.empty(); }
    Tab* ActiveTab();

private:
    int NextId() { return ++next_id_; }

    HWND host_hwnd_;
    std::vector<Tab> tabs_;
    int active_id_ = 0;
    int next_id_   = 0;
};
