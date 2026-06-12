#pragma once

#include "include/cef_browser.h"
#include <windows.h>

class TabManager;
class LumenClient;

class ChromeOverlay {
public:
    ChromeOverlay(HWND parent, TabManager* tab_manager);
    ~ChromeOverlay();

    void Create(int x, int y, int w, int h);
    void SetBounds(int x, int y, int w, int h);

private:
    HWND parent_;
    TabManager* tab_manager_;
    CefRefPtr<LumenClient> client_;
    CefRefPtr<CefBrowser>  browser_;
};
