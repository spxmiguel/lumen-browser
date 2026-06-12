#include "RequestHandler.h"

CefRequestHandler::ReturnValue RequestHandler::OnBeforeResourceLoad(
    CefRefPtr<CefBrowser>,
    CefRefPtr<CefFrame>,
    CefRefPtr<CefRequest> request,
    CefRefPtr<CefCallback> callback) {
    // Phase 3 will wire AdblockEngine + TrackingBlocker here.
    // For now: allow all requests.
    return RV_CONTINUE;
}
