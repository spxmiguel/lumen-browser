#pragma once

#include "include/cef_browser.h"
#include <string>

struct Tab {
    int id = 0;
    CefRefPtr<CefBrowser> browser;
    std::wstring title;
    std::string  url;
    bool         sleeping = false;
};
