#pragma once
#include <windows.h>
#include <cstdint>
#include <atomic>

enum class BrowserMode : uint8_t {
    Normal      = 0,
    Performance = 1,  // max speed, max RAM, all GPU flags on
    Game        = 2,  // 500MB RAM cap, CPU rate-limited, renderer capped
};

struct ModeConfig {
    BrowserMode mode          = BrowserMode::Normal;
    uint64_t    ram_cap_bytes = 0;      // 0 = unlimited
    uint32_t    cpu_rate_pct  = 0;      // 0 = unlimited, 1-100 = hard cap
    uint32_t    tab_sleep_sec = 300;    // idle seconds before tab sleeps
};

class PerformanceMode {
public:
    static PerformanceMode& Get();

    // Call before CefInitialize to get the right CEF command-line flags.
    const ModeConfig& Config() const { return config_; }

    // Apply mode: persists config, applies OS-level limits.
    void Apply(BrowserMode mode);

    // Call after CefInitialize (or at any time) to enforce Game Mode limits.
    void EnforceOSLimits();

    // Remove OS limits (called when switching back to Normal/Performance).
    void ReleaseOSLimits();

private:
    PerformanceMode()  = default;
    ~PerformanceMode() = default;

    void ApplyGameJobObject();
    void LoadPersistedMode();
    void SaveMode(BrowserMode mode);

    ModeConfig config_;
    HANDLE     job_object_ = nullptr;

    static PerformanceMode instance_;
};
