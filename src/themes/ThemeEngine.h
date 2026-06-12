#pragma once

#include "ThemeConfig.h"

// Singleton. Apply() broadcasts theme to all open frames.
// Phase 2 will implement full broadcast + import/export.
class ThemeEngine {
public:
    static ThemeEngine& Get();

    void Apply(const ThemeConfig& config);
    const ThemeConfig& CurrentConfig() const { return current_; }

private:
    ThemeEngine() = default;
    ThemeConfig current_;
};
