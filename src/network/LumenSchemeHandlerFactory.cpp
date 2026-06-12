#include "LumenSchemeHandlerFactory.h"
#include "LumenSchemeHandler.h"

#include "include/cef_request.h"

CefRefPtr<CefResourceHandler> LumenSchemeHandlerFactory::Create(
    CefRefPtr<CefBrowser>,
    CefRefPtr<CefFrame>,
    const CefString&,
    CefRefPtr<CefRequest> request) {
    return new LumenSchemeHandler(request->GetURL());
}
