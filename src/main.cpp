#include "core/BrowserProcess.h"
#include "core/LumenApp.h"
#include "core/LumenRendererApp.h"

#include "include/cef_app.h"
#include "include/cef_command_line.h"

#include <windows.h>

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    CefMainArgs main_args(hInstance);

    // Determine subprocess type before anything else.
    // CEF spawns renderer, GPU, utility subprocesses — they must exit here.
    CefRefPtr<CefApp> subprocess_app = new LumenRendererApp();
    int exit_code = CefExecuteProcess(main_args, subprocess_app, nullptr);
    if (exit_code >= 0)
        return exit_code;

    // Main browser process continues here.
    CefRefPtr<LumenApp> app = new LumenApp();
    return BrowserProcess::Run(main_args, app, hInstance, nCmdShow);
}
