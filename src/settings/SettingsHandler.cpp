#include "SettingsHandler.h"
#include "../features/performance/PerformanceMode.h"

#include <string>

static std::string ModeToString(BrowserMode m) {
    switch (m) {
    case BrowserMode::Performance: return "performance";
    case BrowserMode::Game:        return "game";
    default:                       return "normal";
    }
}

bool SettingsHandler::OnQuery(CefRefPtr<CefBrowser>,
                               CefRefPtr<CefFrame>,
                               int64_t,
                               const CefString& request,
                               bool,
                               CefRefPtr<Callback> callback) {
    const std::string req = request.ToString();

    // perf:get_mode → returns current mode string
    if (req == "perf:get_mode") {
        callback->Success(ModeToString(PerformanceMode::Get().Config().mode));
        return true;
    }

    // perf:set_mode:<normal|performance|game>
    if (req.starts_with("perf:set_mode:")) {
        std::string val = req.substr(14);
        BrowserMode mode = BrowserMode::Normal;
        if (val == "performance") mode = BrowserMode::Performance;
        else if (val == "game")   mode = BrowserMode::Game;
        PerformanceMode::Get().Apply(mode);
        callback->Success("ok:" + val);
        return true;
    }

    // perf:get_config → JSON with current limits
    if (req == "perf:get_config") {
        const auto& cfg = PerformanceMode::Get().Config();
        std::string json = "{\"mode\":\"" + ModeToString(cfg.mode) + "\","
            "\"ram_cap_mb\":" + std::to_string(cfg.ram_cap_bytes / (1024*1024)) + ","
            "\"cpu_rate_pct\":" + std::to_string(cfg.cpu_rate_pct) + ","
            "\"tab_sleep_sec\":" + std::to_string(cfg.tab_sleep_sec) + "}";
        callback->Success(json);
        return true;
    }

    // Fallback stub for unimplemented actions
    callback->Success("ok:" + req);
    return true;
}
