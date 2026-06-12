#pragma once

#include "include/cef_client.h"
#include "include/cef_life_span_handler.h"
#include "include/cef_load_handler.h"
#include "include/cef_display_handler.h"
#include "include/cef_context_menu_handler.h"
#include "include/cef_keyboard_handler.h"
#include "include/cef_download_handler.h"
#include "include/wrapper/cef_message_router.h"

#include "../network/RequestHandler.h"
#include "../settings/SettingsHandler.h"
#include "../ui/TabManager.h"

#include <memory>

class LumenClient : public CefClient,
                    public CefLifeSpanHandler,
                    public CefLoadHandler,
                    public CefDisplayHandler,
                    public CefDownloadHandler,
                    public CefKeyboardHandler {
public:
    explicit LumenClient(TabManager* tab_manager);
    ~LumenClient() override;

    // CefClient — handler accessors
    CefRefPtr<CefLifeSpanHandler>    GetLifeSpanHandler()    override { return this; }
    CefRefPtr<CefLoadHandler>        GetLoadHandler()        override { return this; }
    CefRefPtr<CefDisplayHandler>     GetDisplayHandler()     override { return this; }
    CefRefPtr<CefDownloadHandler>    GetDownloadHandler()    override { return this; }
    CefRefPtr<CefKeyboardHandler>    GetKeyboardHandler()    override { return this; }
    CefRefPtr<CefRequestHandler>     GetRequestHandler()     override;
    bool OnProcessMessageReceived(CefRefPtr<CefBrowser>, CefRefPtr<CefFrame>,
                                  CefProcessId, CefRefPtr<CefProcessMessage>) override;

    // CefLifeSpanHandler
    void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
    bool DoClose(CefRefPtr<CefBrowser> browser) override;
    void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;
    bool OnBeforePopup(CefRefPtr<CefBrowser>, CefRefPtr<CefFrame>,
                       const CefString& target_url, const CefString& target_frame_name,
                       CefLifeSpanHandler::WindowOpenDisposition disposition,
                       bool user_gesture, const CefPopupFeatures&, CefWindowInfo&,
                       CefRefPtr<CefClient>&, CefBrowserSettings&,
                       CefRefPtr<CefDictionaryValue>&, bool*) override;

    // CefLoadHandler
    void OnLoadStart(CefRefPtr<CefBrowser>, CefRefPtr<CefFrame>,
                     TransitionType transition_type) override;
    void OnLoadEnd(CefRefPtr<CefBrowser>, CefRefPtr<CefFrame>, int http_status_code) override;
    void OnLoadError(CefRefPtr<CefBrowser>, CefRefPtr<CefFrame>,
                     ErrorCode error_code, const CefString& error_text,
                     const CefString& failed_url) override;

    // CefDisplayHandler
    void OnTitleChange(CefRefPtr<CefBrowser>, const CefString& title) override;
    void OnFaviconURLChange(CefRefPtr<CefBrowser>, const std::vector<CefString>&) override;
    void OnAddressChange(CefRefPtr<CefBrowser>, CefRefPtr<CefFrame>,
                         const CefString& url) override;

    // CefDownloadHandler
    void OnBeforeDownload(CefRefPtr<CefBrowser>, CefRefPtr<CefDownloadItem>,
                          const CefString& suggested_name,
                          CefRefPtr<CefBeforeDownloadCallback>) override;

    // CefKeyboardHandler
    bool OnPreKeyEvent(CefRefPtr<CefBrowser>, const CefKeyEvent&,
                       CefEventHandle, bool*) override;

    void SetMessageRouter(CefRefPtr<CefMessageRouterBrowserSide> router);

private:
    TabManager* tab_manager_;
    CefRefPtr<RequestHandler> request_handler_;
    CefRefPtr<CefMessageRouterBrowserSide> message_router_;
    std::unique_ptr<SettingsHandler> settings_handler_;

    IMPLEMENT_REFCOUNTING(LumenClient);
};
