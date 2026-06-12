#include "LumenClient.h"

#include "../themes/ThemeEngine.h"
#include "../themes/ThemeInjector.h"
#include "../ui/MainWindow.h"

#include "include/wrapper/cef_helpers.h"
#include "include/cef_app.h"

#include <sstream>

LumenClient::LumenClient(TabManager* tab_manager)
    : tab_manager_(tab_manager),
      request_handler_(new RequestHandler()),
      settings_handler_(std::make_unique<SettingsHandler>()) {
}

LumenClient::~LumenClient() {
    if (message_router_)
        message_router_->RemoveHandler(settings_handler_.get());
}

CefRefPtr<CefRequestHandler> LumenClient::GetRequestHandler() {
    return request_handler_;
}

void LumenClient::SetMessageRouter(CefRefPtr<CefMessageRouterBrowserSide> router) {
    message_router_ = router;
    message_router_->AddHandler(settings_handler_.get(), true);
}

bool LumenClient::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                            CefRefPtr<CefFrame> frame,
                                            CefProcessId source_process,
                                            CefRefPtr<CefProcessMessage> message) {
    if (message_router_)
        return message_router_->OnProcessMessageReceived(browser, frame, source_process, message);
    return false;
}

// --- CefLifeSpanHandler ---

void LumenClient::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();
    if (tab_manager_)
        tab_manager_->OnBrowserCreated(browser);
}

bool LumenClient::DoClose(CefRefPtr<CefBrowser> browser) {
    return false; // allow close
}

void LumenClient::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();
    if (message_router_)
        message_router_->OnBeforeClose(browser);
    if (tab_manager_)
        tab_manager_->OnBrowserClosed(browser);
    if (tab_manager_ && tab_manager_->IsEmpty())
        CefQuitMessageLoop();
}

bool LumenClient::OnBeforePopup(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                  const CefString& target_url, const CefString&,
                                  WindowOpenDisposition, bool, const CefPopupFeatures&,
                                  CefWindowInfo&, CefRefPtr<CefClient>&, CefBrowserSettings&,
                                  CefRefPtr<CefDictionaryValue>&, bool*) {
    // Open popups as new tabs instead of new windows
    if (tab_manager_)
        tab_manager_->OpenTab(target_url);
    return true; // cancel popup window creation
}

// --- CefLoadHandler ---

void LumenClient::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                               TransitionType) {
    if (!frame->IsMain()) return;
    // Notify toolbar overlay of navigation start (show spinner, etc.)
    // TODO: send IPC to ChromeOverlay
}

void LumenClient::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                             int http_status_code) {
    if (!frame->IsMain()) return;
    // Inject theme CSS into every loaded page
    ThemeInjector::InjectIntoFrame(frame, ThemeEngine::Get().CurrentConfig());
}

void LumenClient::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                               ErrorCode error_code, const CefString& error_text,
                               const CefString& failed_url) {
    if (error_code == ERR_ABORTED) return; // navigation cancelled, not an error
    // TODO: show friendly error page
}

// --- CefDisplayHandler ---

void LumenClient::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) {
    // Update window title
    HWND hwnd = browser->GetHost()->GetWindowHandle();
    if (hwnd) {
        std::wstring full_title = title.ToWString() + L" — Lumen";
        SetWindowTextW(hwnd, full_title.c_str());
    }
}

void LumenClient::OnFaviconURLChange(CefRefPtr<CefBrowser> browser,
                                      const std::vector<CefString>& icon_urls) {
    // TODO: notify toolbar to update tab favicon
}

void LumenClient::OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                   const CefString& url) {
    if (!frame->IsMain()) return;
    // TODO: send URL update to toolbar overlay address bar via IPC
}

// --- CefDownloadHandler ---

void LumenClient::OnBeforeDownload(CefRefPtr<CefBrowser>, CefRefPtr<CefDownloadItem>,
                                    const CefString& suggested_name,
                                    CefRefPtr<CefBeforeDownloadCallback> callback) {
    // Auto-accept downloads to default Downloads folder
    callback->Continue(L"", true);
}

// --- CefKeyboardHandler ---

bool LumenClient::OnPreKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent& event,
                                 CefEventHandle, bool*) {
    if (event.type != KEYEVENT_RAWKEYDOWN) return false;

    bool ctrl = (event.modifiers & EVENTFLAG_CONTROL_DOWN) != 0;
    if (!ctrl) return false;

    switch (event.windows_key_code) {
        case 'T': tab_manager_->OpenTab("lumen://newtab"); return true;
        case 'W': tab_manager_->CloseCurrentTab(); return true;
        case 'R': browser->Reload(); return true;
        case 'L': /* TODO: focus address bar */ return true;
        default: return false;
    }
}
