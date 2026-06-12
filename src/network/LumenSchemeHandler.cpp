#include "LumenSchemeHandler.h"

#include "include/cef_response.h"

#include <fstream>
#include <unordered_map>
#include <filesystem>

namespace fs = std::filesystem;

LumenSchemeHandler::LumenSchemeHandler(const CefString& url)
    : url_(url.ToString()) {}

bool LumenSchemeHandler::ProcessRequest(CefRefPtr<CefRequest>,
                                          CefRefPtr<CefCallback> callback) {
    std::string path = ResolveLocalPath(url_);
    mime_ = MimeForPath(path);

    std::ifstream file(path, std::ios::binary);
    if (file) {
        data_ = std::vector<uint8_t>(std::istreambuf_iterator<char>(file), {});
    } else {
        std::string err = "<html><body>404: " + path + " not found</body></html>";
        data_ = std::vector<uint8_t>(err.begin(), err.end());
        mime_ = "text/html";
    }

    callback->Continue();
    return true;
}

void LumenSchemeHandler::GetResponseHeaders(CefRefPtr<CefResponse> response,
                                              int64_t& response_length,
                                              CefString&) {
    response->SetMimeType(mime_);
    response->SetStatus(200);
    response_length = static_cast<int64_t>(data_.size());
}

bool LumenSchemeHandler::ReadResponse(void* data_out, int bytes_to_read,
                                        int& bytes_read, CefRefPtr<CefCallback>) {
    size_t remaining = data_.size() - offset_;
    if (remaining == 0) { bytes_read = 0; return false; }

    size_t to_copy = std::min(static_cast<size_t>(bytes_to_read), remaining);
    memcpy(data_out, data_.data() + offset_, to_copy);
    offset_ += to_copy;
    bytes_read = static_cast<int>(to_copy);
    return true;
}

std::string LumenSchemeHandler::ResolveLocalPath(const std::string& url) const {
    // Strip scheme: lumen://toolbar → toolbar
    std::string host = url;
    const std::string prefix = "lumen://";
    if (host.substr(0, prefix.size()) == prefix)
        host = host.substr(prefix.size());

    // Strip query/fragment
    host = host.substr(0, host.find_first_of("?#"));

    // Remove trailing slash
    if (!host.empty() && host.back() == '/') host.pop_back();

    static const std::unordered_map<std::string, std::string> kRoutes = {
        { "toolbar",   "resources/ui/toolbar.html"          },
        { "newtab",    "resources/ui/newtab.html"            },
        { "settings",  "resources/settings/settings.html"   },
        { "sidebar",   "resources/ui/sidebar.html"           },
        { "onboarding","resources/settings/onboarding.html" },
    };

    // Path-based assets: lumen://toolbar/toolbar.css → resources/ui/toolbar.css
    for (auto& [key, base] : kRoutes) {
        if (host == key) return base;
        if (host.substr(0, key.size() + 1) == key + "/") {
            // Relative asset under the same resource folder
            auto dir = fs::path(base).parent_path();
            return (dir / host.substr(key.size() + 1)).string();
        }
    }

    return "resources/" + host;
}

std::string LumenSchemeHandler::MimeForPath(const std::string& path) const {
    auto ext = fs::path(path).extension().string();
    if (ext == ".html" || ext == ".htm") return "text/html";
    if (ext == ".css")  return "text/css";
    if (ext == ".js")   return "application/javascript";
    if (ext == ".json") return "application/json";
    if (ext == ".svg")  return "image/svg+xml";
    if (ext == ".png")  return "image/png";
    if (ext == ".ico")  return "image/x-icon";
    return "application/octet-stream";
}
