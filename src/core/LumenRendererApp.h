#pragma once

#include "include/cef_app.h"
#include "include/cef_render_process_handler.h"

class LumenRendererApp : public CefApp, public CefRenderProcessHandler {
public:
    LumenRendererApp() = default;

    // CefApp
    CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override { return this; }

    // CefRenderProcessHandler
    void OnWebKitInitialized() override;
    void OnContextCreated(CefRefPtr<CefBrowser> browser,
                          CefRefPtr<CefFrame> frame,
                          CefRefPtr<CefV8Context> context) override;

private:
    IMPLEMENT_REFCOUNTING(LumenRendererApp);
};
