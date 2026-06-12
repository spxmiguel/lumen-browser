#pragma once

#include "include/cef_browser.h"
#include <windows.h>
#include <memory>

class TabManager;
class LumenClient;

class BrowserView {
public:
    BrowserView(HWND parent, TabManager* tab_manager);
    ~BrowserView();

    void Create(int x, int y, int w, int h);
    void SetBounds(int x, int y, int w, int h);

    CefRefPtr<CefBrowser> GetBrowser() const { return browser_; }

private:
    HWND parent_;
    TabManager* tab_manager_;
    CefRefPtr<LumenClient> client_;
    CefRefPtr<CefBrowser>  browser_;
};
