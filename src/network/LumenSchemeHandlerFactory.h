#pragma once

#include "include/cef_scheme.h"

class LumenSchemeHandlerFactory : public CefSchemeHandlerFactory {
public:
    CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser>,
                                          CefRefPtr<CefFrame>,
                                          const CefString& scheme_name,
                                          CefRefPtr<CefRequest> request) override;
private:
    IMPLEMENT_REFCOUNTING(LumenSchemeHandlerFactory);
};
