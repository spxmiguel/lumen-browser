#include "include/cef_app.h"
#include "core/LumenRendererApp.h"

#include <windows.h>

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    CefMainArgs main_args(hInstance);
    CefRefPtr<CefApp> app = new LumenRendererApp();
    return CefExecuteProcess(main_args, app, nullptr);
}
