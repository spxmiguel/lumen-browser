#pragma once

#include "include/cef_request_handler.h"

// Central network intercept point.
// Delegates to AdblockEngine + TrackingBlocker (Phase 3).
class RequestHandler : public CefRequestHandler {
public:
    RequestHandler() = default;

    CefRequestHandler::ReturnValue OnBeforeResourceLoad(
        CefRefPtr<CefBrowser>,
        CefRefPtr<CefFrame>,
        CefRefPtr<CefRequest> request,
        CefRefPtr<CefCallback> callback) override;

private:
    IMPLEMENT_REFCOUNTING(RequestHandler);
};
