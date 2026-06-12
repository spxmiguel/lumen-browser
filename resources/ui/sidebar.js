'use strict';

const frame   = document.getElementById('app-frame');
const appBtns = document.querySelectorAll('.app-btn');

function activate(btn) {
    appBtns.forEach(b => b.classList.remove('active'));
    btn.classList.add('active');
    frame.src = btn.dataset.url;
}

appBtns.forEach(btn => {
    btn.addEventListener('click', () => activate(btn));
});

// Load first app on startup
const first = document.querySelector('.app-btn.active');
if (first) frame.src = first.dataset.url;
