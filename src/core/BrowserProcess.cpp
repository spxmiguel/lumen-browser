#include "BrowserProcess.h"
#include "LumenApp.h"
#include "LumenClient.h"
#include "../ui/MainWindow.h"
#include "../features/performance/PerformanceMode.h"

#include "include/cef_browser.h"
#include "include/cef_command_line.h"

static MainWindow* g_main_window = nullptr;

int BrowserProcess::Run(const CefMainArgs& main_args,
                         CefRefPtr<CefApp> app,
                         HINSTANCE hInstance,
                         int nCmdShow) {
    // Load persisted mode before CEF reads command line (LumenApp queries this)
    PerformanceMode::Get().EnforceOSLimits();

    CefSettings settings;
    settings.no_sandbox = true;
    settings.windowless_rendering_enabled = false;
    CefString(&settings.browser_subprocess_path) = L"LumenBrowserSubprocess.exe";
    CefString(&settings.log_file) = L"lumen_debug.log";
    settings.log_severity = LOGSEVERITY_WARNING;
    CefString(&settings.root_cache_path) = L"userdata";
    CefString(&settings.cache_path) = L"userdata/cache";

    CefInitialize(main_args, settings, app, nullptr);

    CefRunMessageLoop();

    if (g_main_window) {
        delete g_main_window;
        g_main_window = nullptr;
    }

    CefShutdown();
    return 0;
}

void BrowserProcess::CreateMainWindow() {
    g_main_window = new MainWindow();
    g_main_window->Create();
}

void BrowserProcess::Shutdown() {
    if (g_main_window)
        DestroyWindow(g_main_window->GetHWND());
}
