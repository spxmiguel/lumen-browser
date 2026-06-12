# Building Lumen Browser

**Platform:** Windows 10/11 x64 only.

## Prerequisites

| Tool | Version | Where |
|------|---------|--------|
| Visual Studio 2022 | 17+ | With "Desktop development with C++" workload |
| CMake | 3.22+ | cmake.org |
| Ninja | latest | ninja-build.org (or via VS installer) |
| Git | any | git-scm.com |

## 1 — Vendor CEF

Download the **Standard Distribution** for Windows 64-bit from the CEF automated builds:

```
https://cef-builds.spotifycdn.com/index.html
```

Pick **Chromium 120** or newer. Extract and place at:

```
third_party/cef/
```

The directory must contain `libcef.dll`, `libcef.lib`, `libcef_dll_wrapper/`, `include/`, and `Resources/`.

## 2 — Vendor nlohmann/json

Download the single-header release:

```
https://github.com/nlohmann/json/releases/latest
```

Save the downloaded `json.hpp` to:

```
third_party/json/include/nlohmann/json.hpp
```

## 3 — Vendor miniz

Download `miniz.h` and `miniz.c` from:

```
https://github.com/richgel999/miniz/releases/latest
```

Place both files at:

```
third_party/miniz/miniz.h
third_party/miniz/miniz.c
```

## 4 — Download filter lists (optional but recommended)

```
resources/filter-lists/easylist.txt   <- https://easylist.to/easylist/easylist.txt
resources/filter-lists/trackers.txt   <- Disconnect.me domain list (converted)
```

## 5 — Build

Open a **Developer Command Prompt for VS 2022** and run:

```bat
cmake --preset windows-debug
cmake --build build/debug
```

Output: `build/debug/LumenBrowser.exe` + `LumenBrowserSubprocess.exe`

For release:

```bat
cmake --preset windows-release
cmake --build build/release
```

## 6 — Run

```bat
build\debug\LumenBrowser.exe
```

CEF will automatically spawn `LumenBrowserSubprocess.exe` for renderer/GPU processes.

## Private theme (optional)

Place your custom `.lumentheme` file or raw assets in `themes-private/` (gitignored).  
In the browser: **Settings → Appearance → Import .lumentheme**.
