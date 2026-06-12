#include "PerformanceMode.h"

#include <windows.h>
#include <shlobj.h>
#include <fstream>
#include <string>

PerformanceMode PerformanceMode::instance_;

PerformanceMode& PerformanceMode::Get() {
    return instance_;
}

static constexpr uint64_t kGameModeRamCap   = 500ULL * 1024 * 1024;  // 500 MB
static constexpr uint32_t kGameModeCpuRate  = 20;  // max 20% CPU — adjust in settings
static constexpr uint32_t kGameModeTabSleep = 30;  // 30 s idle → tab sleeps

// ---------------------------------------------------------------------------
// Persistence helpers — tiny text file next to the exe
// ---------------------------------------------------------------------------
static std::wstring ConfigPath() {
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    std::wstring s(path);
    auto slash = s.rfind(L'\\');
    if (slash != std::wstring::npos) s = s.substr(0, slash + 1);
    return s + L"userdata\\perf_mode.txt";
}

void PerformanceMode::LoadPersistedMode() {
    std::ifstream f(ConfigPath());
    if (!f) return;
    int m = 0;
    f >> m;
    switch (m) {
    case 1: config_.mode = BrowserMode::Performance; break;
    case 2: config_.mode = BrowserMode::Game;        break;
    default: config_.mode = BrowserMode::Normal;     break;
    }
}

void PerformanceMode::SaveMode(BrowserMode mode) {
    std::ofstream f(ConfigPath());
    f << static_cast<int>(mode);
}

// ---------------------------------------------------------------------------
// Apply
// ---------------------------------------------------------------------------
void PerformanceMode::Apply(BrowserMode mode) {
    ReleaseOSLimits();
    config_.mode = mode;
    SaveMode(mode);

    if (mode == BrowserMode::Game) {
        config_.ram_cap_bytes = kGameModeRamCap;
        config_.cpu_rate_pct  = kGameModeCpuRate;
        config_.tab_sleep_sec = kGameModeTabSleep;
        ApplyGameJobObject();
    } else {
        config_.ram_cap_bytes = 0;
        config_.cpu_rate_pct  = 0;
        config_.tab_sleep_sec = (mode == BrowserMode::Performance) ? 600 : 300;
    }
}

void PerformanceMode::EnforceOSLimits() {
    LoadPersistedMode();
    if (config_.mode == BrowserMode::Game) {
        config_.ram_cap_bytes = kGameModeRamCap;
        config_.cpu_rate_pct  = kGameModeCpuRate;
        config_.tab_sleep_sec = kGameModeTabSleep;
        ApplyGameJobObject();
    }
}

// ---------------------------------------------------------------------------
// Windows Job Object — RAM + CPU limits for Game Mode
// ---------------------------------------------------------------------------
void PerformanceMode::ApplyGameJobObject() {
    if (job_object_) return;  // already applied

    job_object_ = CreateJobObjectW(nullptr, nullptr);
    if (!job_object_) return;

    // Memory cap
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION eli{};
    eli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_PROCESS_MEMORY;
    eli.ProcessMemoryLimit               = static_cast<SIZE_T>(config_.ram_cap_bytes);
    SetInformationJobObject(job_object_,
                            JobObjectExtendedLimitInformation,
                            &eli, sizeof(eli));

    // CPU rate cap (hard, not notify-only)
    JOBOBJECT_CPU_RATE_CONTROL_INFORMATION cri{};
    cri.ControlFlags = JOB_OBJECT_CPU_RATE_CONTROL_ENABLE |
                       JOB_OBJECT_CPU_RATE_CONTROL_HARD_CAP;
    // CpuRate is in units of 1/100 of a percent → multiply by 100
    cri.CpuRate = config_.cpu_rate_pct * 100;
    SetInformationJobObject(job_object_,
                            JobObjectCpuRateControlInformation,
                            &cri, sizeof(cri));

    AssignProcessToJobObject(job_object_, GetCurrentProcess());
}

void PerformanceMode::ReleaseOSLimits() {
    if (!job_object_) return;

    // Remove limits before closing handle
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION eli{};
    eli.BasicLimitInformation.LimitFlags = 0;
    SetInformationJobObject(job_object_,
                            JobObjectExtendedLimitInformation,
                            &eli, sizeof(eli));

    JOBOBJECT_CPU_RATE_CONTROL_INFORMATION cri{};
    cri.ControlFlags = JOB_OBJECT_CPU_RATE_CONTROL_DISABLE;
    SetInformationJobObject(job_object_,
                            JobObjectCpuRateControlInformation,
                            &cri, sizeof(cri));

    CloseHandle(job_object_);
    job_object_ = nullptr;
}
