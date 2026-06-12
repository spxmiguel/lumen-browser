#include "ThemeInjector.h"

#include <sstream>

std::string ThemeInjector::BuildCSSVars(const ThemeConfig& c) {
    std::ostringstream css;
    css << ":root{"
        << "--lumen-toolbar-bg:"       << c.colors.toolbar_bg      << ";"
        << "--lumen-toolbar-text:"     << c.colors.toolbar_text    << ";"
        << "--lumen-tab-active-bg:"    << c.colors.tab_active_bg   << ";"
        << "--lumen-tab-inactive-bg:"  << c.colors.tab_inactive_bg << ";"
        << "--lumen-addressbar-bg:"    << c.colors.addressbar_bg   << ";"
        << "--lumen-accent:"           << c.colors.accent          << ";"
        << "--lumen-toolbar-height:"   << c.geometry.toolbar_height<< "px;"
        << "--lumen-tab-height:"       << c.geometry.tab_height    << "px;"
        << "--lumen-radius:"           << c.geometry.addressbar_border_radius << "px;"
        << "--lumen-toolbar-opacity:"  << c.effects.toolbar_opacity << ";"
        << "}";
    if (!c.css_overrides.empty())
        css << c.css_overrides;
    return css.str();
}

void ThemeInjector::InjectIntoFrame(CefRefPtr<CefFrame> frame,
                                     const ThemeConfig& config) {
    if (!frame || !frame->IsValid()) return;

    std::string css_vars = BuildCSSVars(config);

    // Escape single quotes in CSS for safe injection into JS string literal
    std::string escaped;
    escaped.reserve(css_vars.size());
    for (char ch : css_vars) {
        if (ch == '\'') escaped += "\\'";
        else escaped += ch;
    }

    std::string js = R"js(
(function(){
  var el=document.getElementById('lumen-theme');
  if(!el){el=document.createElement('style');el.id='lumen-theme';document.head.appendChild(el);}
  el.textContent=')js" + escaped + R"js(';
})();
)js";

    frame->ExecuteJavaScript(js, frame->GetURL(), 0);
}
