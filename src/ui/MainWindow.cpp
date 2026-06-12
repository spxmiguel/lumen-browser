#include "MainWindow.h"
#include "TabManager.h"
#include "BrowserView.h"
#include "ChromeOverlay.h"

#include <stdexcept>

static constexpr wchar_t kClassName[] = L"LumenBrowserWindow";

MainWindow::MainWindow() = default;

MainWindow::~MainWindow() {
    // Child views destroyed before HWND
    chrome_overlay_.reset();
    browser_view_.reset();
    tab_manager_.reset();
}

void MainWindow::Create() {
    HINSTANCE hInst = GetModuleHandleW(nullptr);

    WNDCLASSEXW wc{};
    wc.cbSize        = sizeof(wc);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInst;
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = kClassName;
    wc.hIcon         = LoadIcon(nullptr, IDI_APPLICATION);
    RegisterClassExW(&wc);

    hwnd_ = CreateWindowExW(
        0, kClassName, L"Lumen",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1280, 800,
        nullptr, nullptr, hInst, this);

    if (!hwnd_)
        throw std::runtime_error("CreateWindowEx failed");

    ShowWindow(hwnd_, SW_SHOW);
    UpdateWindow(hwnd_);
}

LRESULT CALLBACK MainWindow::WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    MainWindow* self = nullptr;

    if (msg == WM_NCCREATE) {
        auto* cs = reinterpret_cast<CREATESTRUCTW*>(lp);
        self = static_cast<MainWindow*>(cs->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        self->hwnd_ = hwnd;
    } else {
        self = reinterpret_cast<MainWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    }

    if (!self) return DefWindowProcW(hwnd, msg, wp, lp);

    switch (msg) {
        case WM_CREATE: {
            self->OnCreate(hwnd);
            return 0;
        }
        case WM_SIZE: {
            self->OnSize(LOWORD(lp), HIWORD(lp));
            return 0;
        }
        case WM_DESTROY: {
            self->OnDestroy();
            return 0;
        }
        case WM_ERASEBKGND:
            return 1;
    }

    return DefWindowProcW(hwnd, msg, wp, lp);
}

void MainWindow::OnCreate(HWND hwnd) {
    RECT rc;
    GetClientRect(hwnd, &rc);
    int w = rc.right;
    int h = rc.bottom;

    tab_manager_    = std::make_unique<TabManager>(hwnd);
    browser_view_   = std::make_unique<BrowserView>(hwnd, tab_manager_.get());
    chrome_overlay_ = std::make_unique<ChromeOverlay>(hwnd, tab_manager_.get());

    // Content area sits below toolbar
    browser_view_->Create(0, kToolbarHeight, w, h - kToolbarHeight);
    // Toolbar overlay covers top strip, transparent background
    chrome_overlay_->Create(0, 0, w, kToolbarHeight);
}

void MainWindow::OnSize(int width, int height) {
    if (browser_view_)
        browser_view_->SetBounds(0, kToolbarHeight, width, height - kToolbarHeight);
    if (chrome_overlay_)
        chrome_overlay_->SetBounds(0, 0, width, kToolbarHeight);
}

void MainWindow::OnDestroy() {
    PostQuitMessage(0);
}
