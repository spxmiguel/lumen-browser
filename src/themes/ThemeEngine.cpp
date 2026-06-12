#include "ThemeEngine.h"

ThemeEngine& ThemeEngine::Get() {
    static ThemeEngine instance;
    return instance;
}

void ThemeEngine::Apply(const ThemeConfig& config) {
    current_ = config;
    // Phase 2: iterate all open browsers and call ThemeInjector::InjectIntoFrame
}
