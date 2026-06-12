#include "LumenApp.h"
#include "BrowserProcess.h"
#include "../network/LumenSchemeHandlerFactory.h"
#include "../features/performance/PerformanceMode.h"

#include "include/cef_command_line.h"
#include "include/wrapper/cef_helpers.h"

void LumenApp::OnBeforeCommandLineProcessing(const CefString& process_type,
                                              CefRefPtr<CefCommandLine> cmd) {
    cmd->AppendSwitch("no-first-run");
    cmd->AppendSwitch("disable-spell-checking");
    cmd->AppendSwitch("enable-smooth-scrolling");

    const BrowserMode mode = PerformanceMode::Get().Config().mode;

    if (mode == BrowserMode::Performance) {
        // Max throughput — eat all the RAM, saturate the GPU
        cmd->AppendSwitch("enable-zero-copy");
        cmd->AppendSwitch("enable-gpu-compositing");
        cmd->AppendSwitch("enable-accelerated-video-decode");
        cmd->AppendSwitch("enable-accelerated-video-encode");
        cmd->AppendSwitch("disable-background-timer-throttling");
        cmd->AppendSwitch("disable-renderer-backgrounding");
        cmd->AppendSwitch("enable-gpu-rasterization");
        cmd->AppendSwitch("enable-oop-rasterization");
        cmd->AppendSwitch("ignore-gpu-blocklist");
    } else if (mode == BrowserMode::Game) {
        // Minimum footprint — give resources back to the game
        cmd->AppendSwitchWithValue("renderer-process-limit", "1");
        cmd->AppendSwitch("disable-background-networking");
        cmd->AppendSwitch("disable-preconnect");
        cmd->AppendSwitch("disable-translate");
        cmd->AppendSwitch("disable-sync");
        cmd->AppendSwitch("disable-extensions");
    } else {
        // Normal — just enable GPU
        cmd->AppendSwitch("enable-gpu");
    }
}

void LumenApp::OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) {
    // Register lumen:// as a standard, secure, CORS-enabled scheme
    registrar->AddCustomScheme("lumen",
        CEF_SCHEME_OPTION_STANDARD |
        CEF_SCHEME_OPTION_SECURE |
        CEF_SCHEME_OPTION_CORS_ENABLED |
        CEF_SCHEME_OPTION_FETCH_ENABLED);
}

void LumenApp::OnContextInitialized() {
    CEF_REQUIRE_UI_THREAD();

    // Register our lumen:// scheme handler so lumen://toolbar, lumen://newtab, etc. work
    CefRegisterSchemeHandlerFactory("lumen", "",
        new LumenSchemeHandlerFactory());

    // Create the first browser window
    BrowserProcess::CreateMainWindow();
}
