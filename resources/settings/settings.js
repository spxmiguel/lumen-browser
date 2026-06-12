'use strict';

function query(req, cb) {
    window.lumenQuery({ request: req, persistent: false,
        onSuccess(r) { cb && cb(null, r); },
        onFailure(_, m) { cb && cb(m); }
    });
}

// ── Nav routing ──────────────────────────────────────────────────────────────

const navItems = document.querySelectorAll('.nav-item');
const sections = document.querySelectorAll('.settings-section');

navItems.forEach(btn => {
    btn.addEventListener('click', () => {
        navItems.forEach(b => b.classList.remove('active'));
        sections.forEach(s => s.classList.remove('active'));
        btn.classList.add('active');
        const sec = document.getElementById('sec-' + btn.dataset.section);
        if (sec) sec.classList.add('active');
    });
});

// ── Settings load ────────────────────────────────────────────────────────────

query('settings:get', (err, json) => {
    if (err) return;
    try {
        const s = JSON.parse(json);
        setVal('s-homepage',        s.homepage        ?? 'lumen://newtab');
        setVal('s-search-engine',   s.search_engine   ?? 'google');
        setVal('s-downloads',       s.download_path   ?? '');
        setChk('s-adblock',         s.adblock         ?? true);
        setChk('s-antitrack',       s.antitrack       ?? true);
        setChk('s-fingerprint',     s.fingerprint     ?? true);
        setChk('s-perf-mode',       s.perf_mode       ?? false);
        setVal('s-ram-cap',         s.ram_cap_mb      ?? 2048);
        setVal('s-tab-sleep',       s.tab_sleep_min   ?? 5);
        setChk('s-sidebar',         s.sidebar_visible ?? true);
        setVal('s-sidebar-width',   s.sidebar_width   ?? 320);
        setChk('s-toolbar-blur',    s.toolbar_blur    ?? false);
        setVal('s-toolbar-opacity', s.toolbar_opacity ?? 1);
    } catch (_) {}
});

function setVal(id, v) { const el = document.getElementById(id); if (el) el.value = v; }
function setChk(id, v) { const el = document.getElementById(id); if (el) el.checked = v; }

// ── Settings save on change ──────────────────────────────────────────────────

function collectAndSave() {
    const s = {
        homepage:        document.getElementById('s-homepage')?.value,
        search_engine:   document.getElementById('s-search-engine')?.value,
        adblock:         document.getElementById('s-adblock')?.checked,
        antitrack:       document.getElementById('s-antitrack')?.checked,
        fingerprint:     document.getElementById('s-fingerprint')?.checked,
        perf_mode:       document.getElementById('s-perf-mode')?.checked,
        ram_cap_mb:      Number(document.getElementById('s-ram-cap')?.value),
        tab_sleep_min:   Number(document.getElementById('s-tab-sleep')?.value),
        sidebar_visible: document.getElementById('s-sidebar')?.checked,
        sidebar_width:   Number(document.getElementById('s-sidebar-width')?.value),
        toolbar_blur:    document.getElementById('s-toolbar-blur')?.checked,
        toolbar_opacity: Number(document.getElementById('s-toolbar-opacity')?.value),
    };
    query('settings:set:' + JSON.stringify(s));
}

document.getElementById('settings-content').addEventListener('change', collectAndSave);
document.getElementById('settings-content').addEventListener('input',  collectAndSave);

// ── Button actions ───────────────────────────────────────────────────────────

document.getElementById('btn-chrome-settings')
    ?.addEventListener('click', () => query('navigate:chrome://settings'));
document.getElementById('btn-chrome-flags')
    ?.addEventListener('click', () => query('navigate:chrome://flags'));
document.getElementById('btn-devtools')
    ?.addEventListener('click', () => query('devtools:open'));
document.getElementById('btn-import-theme')
    ?.addEventListener('click', () => query('theme:import'));
document.getElementById('btn-export-theme')
    ?.addEventListener('click', () => query('theme:export'));
