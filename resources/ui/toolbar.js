'use strict';

const addrBar = document.getElementById('address-bar');
const btnBack = document.getElementById('btn-back');
const btnFwd  = document.getElementById('btn-forward');
const btnRld  = document.getElementById('btn-reload');
const tabBar  = document.getElementById('tab-bar');
const newTabBtn = document.getElementById('new-tab-btn');

function query(req, cb) {
    window.lumenQuery({ request: req, persistent: false,
        onSuccess(r) { cb && cb(null, r); },
        onFailure(c, m) { cb && cb(m); }
    });
}

// ── Navigation ───────────────────────────────────────────────────────────────

addrBar.addEventListener('keydown', e => {
    if (e.key !== 'Enter') return;
    let url = addrBar.value.trim();
    if (!url) return;
    if (!url.includes('.') && !url.startsWith('lumen://') && !url.startsWith('chrome://')) {
        url = 'https://www.google.com/search?q=' + encodeURIComponent(url);
    } else if (!/^[a-z]+:\/\//i.test(url)) {
        url = 'https://' + url;
    }
    query('navigate:' + url);
    addrBar.blur();
});

btnBack.addEventListener('click',    () => query('nav:back'));
btnFwd.addEventListener('click',     () => query('nav:forward'));
btnRld.addEventListener('click',     () => query('nav:reload'));
newTabBtn.addEventListener('click',  () => query('tab:new'));

document.getElementById('btn-settings').addEventListener('click',
    () => query('navigate:lumen://settings'));
document.getElementById('btn-sidebar').addEventListener('click',
    () => query('sidebar:toggle'));

// ── Tab management ────────────────────────────────────────────────────────────

function addTabEl(id, title) {
    const tab = document.createElement('div');
    tab.className = 'tab';
    tab.dataset.id = id;
    tab.innerHTML = `<img class="favicon" src="" alt=""><span class="tab-title">${escHtml(title)}</span><button class="tab-close" title="Close">✕</button>`;
    tab.addEventListener('click', e => {
        if (e.target.classList.contains('tab-close')) {
            query('tab:close:' + id);
        } else {
            query('tab:switch:' + id);
        }
    });
    tabBar.insertBefore(tab, newTabBtn);
    return tab;
}

function escHtml(s) {
    return s.replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;');
}

// ── Messages from C++ ─────────────────────────────────────────────────────────

window.lumenOnTabCreated = (id) => addTabEl(id, 'New Tab');

window.lumenOnTabClosed = (id) => {
    const el = tabBar.querySelector(`.tab[data-id="${id}"]`);
    if (el) el.remove();
};

window.lumenOnTabActivated = (id) => {
    tabBar.querySelectorAll('.tab').forEach(t =>
        t.classList.toggle('active', t.dataset.id == id));
};

window.lumenOnTabTitleChanged = (id, title) => {
    const el = tabBar.querySelector(`.tab[data-id="${id}"] .tab-title`);
    if (el) el.textContent = title;
};

window.lumenOnTabFaviconChanged = (id, url) => {
    const el = tabBar.querySelector(`.tab[data-id="${id}"] .favicon`);
    if (el) el.src = url;
};

window.lumenOnUrlChanged = (url) => {
    if (document.activeElement !== addrBar) addrBar.value = url;
};

window.lumenOnNavState = (canBack, canFwd) => {
    btnBack.disabled = !canBack;
    btnFwd.disabled  = !canFwd;
};
