#pragma once

#include <string>

struct ThemeColors {
    std::string toolbar_bg       = "#1c1c1e";
    std::string toolbar_text     = "#ffffff";
    std::string tab_active_bg    = "#2c2c2e";
    std::string tab_inactive_bg  = "#1c1c1e";
    std::string addressbar_bg    = "#2c2c2e";
    std::string accent           = "#0a84ff";
};

struct ThemeGeometry {
    int    toolbar_height            = 52;
    int    tab_height                = 36;
    int    addressbar_border_radius  = 8;
    int    sidebar_width             = 320;
};

struct ThemeEffects {
    bool   toolbar_blur        = false;
    int    toolbar_blur_amount = 20;
    double toolbar_opacity     = 1.0;
};

struct ThemeNewtab {
    std::string background_image;
    bool show_search_bar = true;
    bool show_clock      = true;
};

struct ThemeConfig {
    std::string  name    = "Default";
    std::string  version = "1.0.0";
    ThemeColors  colors;
    ThemeGeometry geometry;
    ThemeEffects  effects;
    ThemeNewtab   newtab;
    std::string   css_overrides;
};
