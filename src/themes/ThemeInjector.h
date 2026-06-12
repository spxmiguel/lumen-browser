#pragma once

#include "ThemeConfig.h"
#include "include/cef_frame.h"

class ThemeInjector {
public:
    // Builds a CSS custom-property block and injects it into the frame.
    // Idempotent — replaces existing #lumen-theme <style> if present.
    static void InjectIntoFrame(CefRefPtr<CefFrame> frame,
                                 const ThemeConfig& config);

    static std::string BuildCSSVars(const ThemeConfig& config);
};
