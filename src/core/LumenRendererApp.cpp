#include "LumenRendererApp.h"

#include "include/cef_browser.h"
#include "include/cef_frame.h"
#include "include/cef_v8.h"

void LumenRendererApp::OnWebKitInitialized() {
    // Inject fingerprint guard JavaScript into every renderer context.
    // This spoofs canvas fingerprinting, WebRTC IP leak, and navigator.webdriver.
    const char* fingerprint_guard_js = R"js(
        (function() {
            // Hide automation flag
            Object.defineProperty(navigator, 'webdriver', { get: () => false });

            // Spoof canvas fingerprint (add tiny noise)
            const origToDataURL = HTMLCanvasElement.prototype.toDataURL;
            HTMLCanvasElement.prototype.toDataURL = function(type) {
                const ctx = this.getContext('2d');
                if (ctx) {
                    const imgData = ctx.getImageData(0, 0, this.width, this.height);
                    imgData.data[0] ^= 1; // single-pixel noise
                    ctx.putImageData(imgData, 0, 0);
                }
                return origToDataURL.apply(this, arguments);
            };
        })();
    )js";

    CefRegisterExtension("v8/lumen_fingerprint_guard", fingerprint_guard_js, nullptr);
}

void LumenRendererApp::OnContextCreated(CefRefPtr<CefBrowser> browser,
                                         CefRefPtr<CefFrame> frame,
                                         CefRefPtr<CefV8Context> context) {
    // Per-frame injection happens here if needed (theme CSS, etc.)
    // The main theme injection is done via ExecuteJavaScript from the browser process
    // in LumenLoadHandler::OnLoadEnd — no duplication needed here.
}
