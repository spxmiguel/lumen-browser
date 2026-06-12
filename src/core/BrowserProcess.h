#pragma once

#include "include/cef_app.h"
#include "include/cef_base.h"

#include <windows.h>

class BrowserProcess {
public:
    static int Run(const CefMainArgs& main_args,
                   CefRefPtr<CefApp> app,
                   HINSTANCE hInstance,
                   int nCmdShow);

    static void CreateMainWindow();

    static void Shutdown();

private:
    BrowserProcess() = delete;
};
