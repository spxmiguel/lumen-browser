#pragma once

#include <windows.h>
#include <memory>

class TabManager;
class BrowserView;
class ChromeOverlay;

class MainWindow {
public:
    MainWindow();
    ~MainWindow();

    void Create();
    HWND GetHWND() const { return hwnd_; }

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

private:
    void OnCreate(HWND hwnd);
    void OnSize(int width, int height);
    void OnDestroy();

    static constexpr int kToolbarHeight = 88;  // 36 tab bar + 52 nav bar
    static constexpr int kSidebarWidth  = 0; // 0 = hidden by default

    HWND hwnd_ = nullptr;
    std::unique_ptr<TabManager>    tab_manager_;
    std::unique_ptr<BrowserView>   browser_view_;
    std::unique_ptr<ChromeOverlay> chrome_overlay_;
};
