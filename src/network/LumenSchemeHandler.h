#pragma once

#include "include/cef_resource_handler.h"
#include "include/cef_response.h"

#include <string>
#include <vector>

// Serves lumen:// URLs from resources/ on disk.
// URL map:
//   lumen://toolbar    → resources/ui/toolbar.html
//   lumen://newtab     → resources/ui/newtab.html
//   lumen://settings   → resources/settings/settings.html
//   lumen://sidebar    → resources/ui/sidebar.html
class LumenSchemeHandler : public CefResourceHandler {
public:
    explicit LumenSchemeHandler(const CefString& url);

    bool ProcessRequest(CefRefPtr<CefRequest>, CefRefPtr<CefCallback>) override;
    void GetResponseHeaders(CefRefPtr<CefResponse>, int64_t& response_length,
                             CefString& redirect_url) override;
    bool ReadResponse(void* data_out, int bytes_to_read,
                      int& bytes_read, CefRefPtr<CefCallback>) override;
    void Cancel() override {}

private:
    std::string ResolveLocalPath(const std::string& url) const;
    std::string MimeForPath(const std::string& path) const;

    std::string url_;
    std::vector<uint8_t> data_;
    size_t offset_ = 0;
    std::string mime_;

    IMPLEMENT_REFCOUNTING(LumenSchemeHandler);
};
