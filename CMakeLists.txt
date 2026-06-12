cmake_minimum_required(VERSION 3.22)
project(LumensBrowser CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(CompilerFlags)
include(FindCEF)
include(CopyResources)

# ── Browser process (main exe) ────────────────────────────────────────────────

set(BROWSER_SOURCES
    src/main.cpp

    src/core/LumenApp.cpp
    src/core/LumenRendererApp.cpp
    src/core/LumenClient.cpp
    src/core/BrowserProcess.cpp

    src/ui/MainWindow.cpp
    src/ui/BrowserView.cpp
    src/ui/ChromeOverlay.cpp
    src/ui/TabManager.cpp

    src/network/LumenSchemeHandlerFactory.cpp
    src/network/LumenSchemeHandler.cpp
    src/network/RequestHandler.cpp

    src/settings/SettingsHandler.cpp

    src/themes/ThemeEngine.cpp
    src/themes/ThemeInjector.cpp

    src/features/performance/PerformanceMode.cpp
)

add_executable(LumenBrowser WIN32 ${BROWSER_SOURCES})

target_include_directories(LumenBrowser PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/third_party/json/include"
    "${CMAKE_SOURCE_DIR}/third_party/miniz"
)

target_link_libraries(LumenBrowser PRIVATE CEF::cef)

lumen_copy_cef_resources(LumenBrowser)

# ── Subprocess exe (renderer / GPU / utility) ─────────────────────────────────
# CEF spawns this automatically; it must exist alongside LumenBrowser.exe.

add_executable(LumenBrowserSubprocess WIN32
    src/subprocess_main.cpp
)

target_include_directories(LumenBrowserSubprocess PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
)

target_link_libraries(LumenBrowserSubprocess PRIVATE CEF::cef)
