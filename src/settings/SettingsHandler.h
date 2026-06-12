#pragma once

#include "include/wrapper/cef_message_router.h"

// Handles lumenQuery() calls from toolbar/settings HTML.
// Phase 6 will implement full settings persistence here.
class SettingsHandler : public CefMessageRouterBrowserSide::Handler {
public:
    bool OnQuery(CefRefPtr<CefBrowser>, CefRefPtr<CefFrame>,
                 int64_t query_id, const CefString& request,
                 bool persistent, CefRefPtr<Callback> callback) override;

    void OnQueryCanceled(CefRefPtr<CefBrowser>, CefRefPtr<CefFrame>,
                         int64_t query_id) override {}
};
