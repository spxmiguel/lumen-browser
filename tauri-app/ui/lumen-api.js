/**
 * lumen-api.js — Tauri v2 shim that exposes window.lumen
 * identical API to the old Electron preload.js.
 * Loaded by toolbar.html and newtab.html.
 */

(function () {
  'use strict';

  // Tauri v2 global (set by withGlobalTauri: true in tauri.conf.json)
  const invoke = (...args) => window.__TAURI__.core.invoke(...args);
  const listen = (...args) => window.__TAURI__.event.listen(...args);

  const lumen = {
    // ── Navigation ────────────────────────────────────────────
    navigate:    (url) => invoke('navigate_cmd',    { url }),
    goBack:      ()    => invoke('go_back_cmd'),
    goForward:   ()    => invoke('go_forward_cmd'),
    reload:      ()    => invoke('reload_cmd'),
    stop:        ()    => { /* WKWebView doesn't expose stop via Tauri; no-op */ },
    getNavState: ()    => invoke('get_nav_state_cmd'),

    // ── Tabs ──────────────────────────────────────────────────
    newTab:    (url) => invoke('tab_new_cmd',    { url: url || null }),
    switchTab: (id)  => invoke('tab_switch_cmd', { id }),
    closeTab:  (id)  => invoke('tab_close_cmd',  { id }),
    getTabs:   ()    => invoke('get_tabs_cmd'),

    // ── Window Controls ───────────────────────────────────────
    closeWindow:    () => invoke('plugin:window|close'),
    minimizeWindow: () => invoke('plugin:window|minimize'),
    maximizeWindow: () => invoke('plugin:window|toggle_maximize'),

    // ── Game Mode ─────────────────────────────────────────────
    setGameMode: (enabled) => invoke('set_game_mode_cmd', { enabled }),
    getGameMode: ()        => invoke('get_game_mode_cmd'),

    // ── Notify Rust of tab metadata changes ───────────────────
    updateTitle: (id, title, url) => invoke('tab_title_update_cmd', { id, title, url: url || '' }),
    setLoading:  (id, loading)    => invoke('tab_loading_cmd', { id, loading }),

    // ── Events ────────────────────────────────────────────────
    onTabsUpdate: (cb) => {
      listen('tabs-update', (event) => {
        const { tabs, navState } = event.payload;
        const active = tabs.find(t => t.active);
        cb(tabs, active?.url || '', navState);
      });
    },

    onLoadStart: (cb) => {
      // Fired by content webview navigation-started (injected via eval)
      listen('load-start', () => cb());
    },

    onLoadStop: (cb) => {
      listen('load-stop', () => cb());
    },

    onGameModeChanged: (cb) => {
      listen('game-mode-changed', (event) => cb(event.payload));
    },
  };

  window.lumen = lumen;
})();
