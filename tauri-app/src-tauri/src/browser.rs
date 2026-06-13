use std::collections::HashMap;
use std::sync::{Arc, Mutex};
use serde::{Deserialize, Serialize};
use tauri::{AppHandle, Emitter, Manager, WebviewUrl};
use url::Url;

// ─── AD BLOCK ────────────────────────────────────────────────────────────────

#[allow(dead_code)]
static AD_DOMAINS: &[&str] = &[
    "doubleclick.net","googlesyndication.com","googleadservices.com",
    "adnxs.com","advertising.com","taboola.com","outbrain.com",
    "media.net","pubmatic.com","rubiconproject.com","openx.net",
    "appnexus.com","criteo.com","adroll.com","scorecardresearch.com",
    "quantserve.com","hotjar.com","mouseflow.com","fullstory.com",
    "mixpanel.com","moatads.com","amazon-adsystem.com","adsrvr.org",
    "casalemedia.com","bidswitch.net","rlcdn.com","bluekai.com",
    "demdex.net","krxd.net","sharethrough.com","triplelift.com",
    "sovrn.com","indexexchange.com","33across.com","spotxchange.com",
];

#[allow(dead_code)]
pub fn is_ad_domain(hostname: &str) -> bool {
    let host = hostname.trim_start_matches("www.");
    AD_DOMAINS.iter().any(|&ad| host == ad || host.ends_with(&format!(".{}", ad)))
}

// ─── URL NORMALIZE ────────────────────────────────────────────────────────────

pub fn normalize_url(raw: &str) -> String {
    let s = raw.trim();
    if s.is_empty() { return "lumen://newtab".to_string(); }
    if s.starts_with("lumen://") { return s.to_string(); }
    if s.starts_with("http://") || s.starts_with("https://") { return s.to_string(); }
    if s.starts_with("localhost") || s.starts_with("127.0.0.1") {
        return format!("http://{}", s);
    }
    let looks_like_domain = s.contains('.') && !s.contains(' ')
        && s.split('.').last().map(|p| p.len() >= 2).unwrap_or(false);
    if looks_like_domain { return format!("https://{}", s); }
    // percent-encode for search
    let encoded: String = s.bytes().map(|b| match b {
        b'A'..=b'Z'|b'a'..=b'z'|b'0'..=b'9'|b'-'|b'_'|b'.'|b'~' => (b as char).to_string(),
        b' ' => "+".to_string(),
        _ => format!("%{:02X}", b),
    }).collect();
    format!("https://www.google.com/search?q={}", encoded)
}

// ─── STATE ───────────────────────────────────────────────────────────────────

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TabInfo {
    pub id: u32,
    pub title: String,
    pub url: String,
    pub favicon: Option<String>,
    pub active: bool,
    pub loading: bool,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct NavState {
    #[serde(rename = "canGoBack")]
    pub can_go_back: bool,
    #[serde(rename = "canGoForward")]
    pub can_go_forward: bool,
    pub url: String,
}

#[derive(Debug)]
pub struct Tab {
    pub id: u32,
    pub url: String,
    pub title: String,
    pub favicon: Option<String>,
    pub loading: bool,
    pub history: Vec<String>,
    pub history_idx: usize,
}

impl Tab {
    fn new(id: u32, url: &str) -> Self {
        Tab {
            id, url: url.to_string(), title: "New Tab".to_string(),
            favicon: None, loading: false,
            history: vec![url.to_string()], history_idx: 0,
        }
    }
    fn can_go_back(&self)    -> bool { self.history_idx > 0 }
    fn can_go_forward(&self) -> bool { self.history_idx + 1 < self.history.len() }

    fn push_url(&mut self, url: &str) {
        self.history.truncate(self.history_idx + 1);
        self.history.push(url.to_string());
        self.history_idx = self.history.len() - 1;
        self.url = url.to_string();
    }
    fn go_back(&mut self) -> Option<String> {
        if self.can_go_back() { self.history_idx -= 1; self.url = self.history[self.history_idx].clone(); Some(self.url.clone()) } else { None }
    }
    fn go_forward(&mut self) -> Option<String> {
        if self.can_go_forward() { self.history_idx += 1; self.url = self.history[self.history_idx].clone(); Some(self.url.clone()) } else { None }
    }
}

#[derive(Debug, Default)]
pub struct BrowserState {
    pub tabs: HashMap<u32, Tab>,
    pub active_tab_id: u32,
    pub tab_counter: u32,
    pub game_mode: bool,
}

impl BrowserState {
    pub fn new_tab_entry(&mut self, url: &str) -> u32 {
        self.tab_counter += 1;
        let id = self.tab_counter;
        self.tabs.insert(id, Tab::new(id, url));
        id
    }
    pub fn active_tab(&self)     -> Option<&Tab>     { self.tabs.get(&self.active_tab_id) }
    pub fn active_tab_mut(&mut self) -> Option<&mut Tab> { self.tabs.get_mut(&self.active_tab_id) }

    pub fn snapshot(&self) -> (Vec<TabInfo>, NavState) {
        let mut list: Vec<TabInfo> = self.tabs.values().map(|t| TabInfo {
            id: t.id, title: t.title.clone(), url: t.url.clone(),
            favicon: t.favicon.clone(), active: t.id == self.active_tab_id, loading: t.loading,
        }).collect();
        list.sort_by_key(|t| t.id);
        let nav = match self.active_tab() {
            Some(t) => NavState { can_go_back: t.can_go_back(), can_go_forward: t.can_go_forward(), url: t.url.clone() },
            None    => NavState { can_go_back: false, can_go_forward: false, url: String::new() },
        };
        (list, nav)
    }
}

pub type State = Arc<Mutex<BrowserState>>;

// ─── NAVIGATE HELPER ─────────────────────────────────────────────────────────

fn load_in_content(app: &AppHandle, url: &str) -> Result<(), String> {
    let webview = app.get_webview("content")
        .ok_or_else(|| "content webview not found".to_string())?;

    // Tauri v2: navigate() takes url::Url. For local pages use tauri://localhost/
    let nav_url: Url = if url == "lumen://newtab" {
        "tauri://localhost/newtab.html".parse().unwrap()
    } else {
        url.parse().map_err(|e: url::ParseError| format!("bad url: {}", e))?
    };

    webview.navigate(nav_url).map_err(|e| format!("{}", e))
}

fn emit_update(app: &AppHandle, state: &State) {
    let st = state.lock().unwrap();
    let (tabs, nav) = st.snapshot();
    drop(st);
    let _ = app.emit("tabs-update", serde_json::json!({ "tabs": tabs, "navState": nav }));
}

// ─── COMMANDS ────────────────────────────────────────────────────────────────

#[tauri::command]
pub fn navigate_cmd(
    app: AppHandle,
    state: tauri::State<'_, State>,
    url: String,
) -> Result<(), String> {
    let normalized = normalize_url(&url);
    { let mut st = state.lock().unwrap(); if let Some(t) = st.active_tab_mut() { t.push_url(&normalized); } }
    load_in_content(&app, &normalized)?;
    emit_update(&app, &state);
    Ok(())
}

#[tauri::command]
pub fn go_back_cmd(app: AppHandle, state: tauri::State<'_, State>) -> Result<(), String> {
    let url = { let mut st = state.lock().unwrap(); st.active_tab_mut().and_then(|t| t.go_back()) };
    if let Some(u) = url { load_in_content(&app, &u)?; }
    emit_update(&app, &state);
    Ok(())
}

#[tauri::command]
pub fn go_forward_cmd(app: AppHandle, state: tauri::State<'_, State>) -> Result<(), String> {
    let url = { let mut st = state.lock().unwrap(); st.active_tab_mut().and_then(|t| t.go_forward()) };
    if let Some(u) = url { load_in_content(&app, &u)?; }
    emit_update(&app, &state);
    Ok(())
}

#[tauri::command]
pub fn reload_cmd(app: AppHandle, state: tauri::State<'_, State>) -> Result<(), String> {
    let url = { state.lock().unwrap().active_tab().map(|t| t.url.clone()) };
    if let Some(u) = url { load_in_content(&app, &u)?; }
    Ok(())
}

#[tauri::command]
pub fn get_nav_state_cmd(state: tauri::State<'_, State>) -> NavState {
    let st = state.lock().unwrap();
    match st.active_tab() {
        Some(t) => NavState { can_go_back: t.can_go_back(), can_go_forward: t.can_go_forward(), url: t.url.clone() },
        None    => NavState { can_go_back: false, can_go_forward: false, url: String::new() },
    }
}

#[tauri::command]
pub fn tab_new_cmd(
    app: AppHandle, state: tauri::State<'_, State>, url: Option<String>,
) -> Result<(), String> {
    let load_url = url.unwrap_or_else(|| "lumen://newtab".to_string());
    { let mut st = state.lock().unwrap(); let id = st.new_tab_entry(&load_url); st.active_tab_id = id; }
    load_in_content(&app, &load_url)?;
    emit_update(&app, &state);
    Ok(())
}

#[tauri::command]
pub fn tab_switch_cmd(
    app: AppHandle, state: tauri::State<'_, State>, id: u32,
) -> Result<(), String> {
    let url = {
        let mut st = state.lock().unwrap();
        if !st.tabs.contains_key(&id) { return Err("tab not found".into()); }
        st.active_tab_id = id;
        st.tabs[&id].url.clone()
    };
    load_in_content(&app, &url)?;
    emit_update(&app, &state);
    Ok(())
}

#[tauri::command]
pub fn tab_close_cmd(
    app: AppHandle, state: tauri::State<'_, State>, id: u32,
) -> Result<(), String> {
    let new_url = {
        let mut st = state.lock().unwrap();
        st.tabs.remove(&id);
        if st.tabs.is_empty() {
            let nid = st.new_tab_entry("lumen://newtab");
            st.active_tab_id = nid;
            "lumen://newtab".to_string()
        } else if st.active_tab_id == id {
            let last = *st.tabs.keys().max().unwrap();
            st.active_tab_id = last;
            st.tabs[&last].url.clone()
        } else {
            st.active_tab().map(|t| t.url.clone()).unwrap_or_default()
        }
    };
    load_in_content(&app, &new_url)?;
    emit_update(&app, &state);
    Ok(())
}

#[tauri::command]
pub fn get_tabs_cmd(state: tauri::State<'_, State>) -> Vec<TabInfo> {
    state.lock().unwrap().snapshot().0
}

#[tauri::command]
pub fn tab_title_update_cmd(
    app: AppHandle, state: tauri::State<'_, State>,
    id: u32, title: String, url: String,
) {
    { let mut st = state.lock().unwrap(); if let Some(t) = st.tabs.get_mut(&id) { t.title = title; if !url.is_empty() { t.url = url; } } }
    emit_update(&app, &state);
}

#[tauri::command]
pub fn tab_loading_cmd(
    app: AppHandle, state: tauri::State<'_, State>, id: u32, loading: bool,
) {
    { let mut st = state.lock().unwrap(); if let Some(t) = st.tabs.get_mut(&id) { t.loading = loading; } }
    emit_update(&app, &state);
}

#[tauri::command]
pub fn set_game_mode_cmd(
    app: AppHandle, state: tauri::State<'_, State>, enabled: bool,
) {
    { state.lock().unwrap().game_mode = enabled; }
    let _ = app.emit("game-mode-changed", enabled);

    // macOS: lower process niceness so games get more CPU
    #[cfg(target_os = "macos")]
    unsafe {
        libc::setpriority(libc::PRIO_PROCESS, 0, if enabled { 15 } else { 0 });
    }

    // Throttle JS in content webview when game mode on
    if let Some(wv) = app.get_webview("content") {
        let js = if enabled {
            // Heavily throttle rAF and intervals
            r#"
            (function(){
                if(window._lumenGameMode) return;
                window._lumenGameMode = true;
                var _raf = window.requestAnimationFrame;
                window.requestAnimationFrame = function(cb){ return setTimeout(cb, 500); };
                console.log('[Lumen] Game mode ON — JS throttled');
            })();
            "#
        } else {
            r#"
            (function(){
                window._lumenGameMode = false;
                // restore native rAF (page refresh needed for full restore)
                console.log('[Lumen] Game mode OFF');
            })();
            "#
        };
        let _ = wv.eval(js);
    }
}

#[tauri::command]
pub fn get_game_mode_cmd(state: tauri::State<'_, State>) -> bool {
    state.lock().unwrap().game_mode
}

// ─── APP RUN ─────────────────────────────────────────────────────────────────

const TOOLBAR_H: f64 = 85.0;

pub fn run() {
    let browser_state: State = Arc::new(Mutex::new(BrowserState::default()));
    {
        let mut st = browser_state.lock().unwrap();
        let id = st.new_tab_entry("https://www.google.com");
        st.active_tab_id = id;
    }

    tauri::Builder::default()
        .manage(browser_state)
        .plugin(tauri_plugin_shell::init())
        .invoke_handler(tauri::generate_handler![
            navigate_cmd, go_back_cmd, go_forward_cmd, reload_cmd,
            get_nav_state_cmd, tab_new_cmd, tab_switch_cmd, tab_close_cmd,
            get_tabs_cmd, tab_title_update_cmd, tab_loading_cmd,
            set_game_mode_cmd, get_game_mode_cmd,
        ])
        .setup(|app| {
            let handle = app.handle().clone();

            // ── Bare window (no webview, just the OS frame) ──────────────
            // macOS: decorations(false) + custom HTML traffic lights in toolbar.html.
            // TitleBarStyle::Overlay was tried but native NSButtons are hidden behind
            // the toolbar child webview in Tauri's multi-webview layout.
            #[cfg(target_os = "macos")]
            let win = tauri::window::WindowBuilder::new(app, "main")
                .title("")
                .inner_size(1280.0, 820.0)
                .min_inner_size(700.0, 400.0)
                .decorations(false)
                .transparent(true)
                .center()
                .build()?;
            #[cfg(not(target_os = "macos"))]
            let win = tauri::window::WindowBuilder::new(app, "main")
                .title("")
                .inner_size(1280.0, 820.0)
                .min_inner_size(700.0, 400.0)
                .decorations(false)
                .transparent(true)
                .build()?;

            let (win_w, win_h) = {
                let s = win.inner_size()?;
                (s.width as f64, s.height as f64)
            };

            // ── Toolbar child webview ────────────────────────────────────
            win.add_child(
                tauri::WebviewBuilder::new("toolbar", WebviewUrl::App("toolbar.html".into()))
                    .transparent(true),
                tauri::LogicalPosition::new(0.0, 0.0),
                tauri::LogicalSize::new(win_w, TOOLBAR_H),
            )?;

            // ── Content child webview ────────────────────────────────────
            win.add_child(
                tauri::WebviewBuilder::new(
                    "content",
                    WebviewUrl::External("https://www.google.com".parse().unwrap()),
                ),
                tauri::LogicalPosition::new(0.0, TOOLBAR_H),
                tauri::LogicalSize::new(win_w, win_h - TOOLBAR_H),
            )?;

            // ── Resize handler — keep child views sized correctly ────────
            win.on_window_event(move |event| {
                if let tauri::WindowEvent::Resized(size) = event {
                    let w = size.width as f64;
                    let h = size.height as f64;

                    if let Some(tv) = handle.get_webview("toolbar") {
                        let _ = tv.set_bounds(tauri::Rect {
                            position: tauri::LogicalPosition::new(0.0, 0.0).into(),
                            size: tauri::LogicalSize::new(w, TOOLBAR_H).into(),
                        });
                    }
                    if let Some(cv) = handle.get_webview("content") {
                        let _ = cv.set_bounds(tauri::Rect {
                            position: tauri::LogicalPosition::new(0.0, TOOLBAR_H).into(),
                            size: tauri::LogicalSize::new(w, h - TOOLBAR_H).into(),
                        });
                    }
                }
            });

            // ── Push initial tab state after a short delay ───────────────
            // Toolbar polls via getTabs()/getGameMode() on DOMContentLoaded,
            // but we also emit once to ensure it gets the state.
            let h2 = app.handle().clone();
            let state2 = app.state::<State>().inner().clone();
            std::thread::spawn(move || {
                std::thread::sleep(std::time::Duration::from_millis(500));
                emit_update(&h2, &state2);
            });

            win.show()?;
            Ok(())
        })
        .run(tauri::generate_context!())
        .expect("Lumen Browser runtime error");
}
