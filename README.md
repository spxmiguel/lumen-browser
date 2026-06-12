# Lumen Browser

A fast, private, fully customizable Chromium-based browser for Windows.

## Features

- **Theme engine** — reskin any element via JSON + CSS + PNG. Import/export `.lumentheme` bundles.
- **Native ad blocker** — EasyList-based, enabled by default.
- **Anti-tracking** — aggressive tracker and fingerprint protection, enabled by default.
- **Performance mode** — user-defined RAM/CPU caps, automatic tab sleep for idle tabs.
- **Sidebar** — optional pinnable panel for web apps (AI assistants, messaging, music).
- **Multi-tab** — full tab bar with keyboard shortcuts (Ctrl+T, Ctrl+W, Ctrl+R).
- **Search engines** — Google, DuckDuckGo, Bing, Yahoo, Brave Search — user's choice.
- **Chromium internals** — direct access to `chrome://settings` and `chrome://flags` when needed.

## Architecture

Built on [CEF3](https://bitbucket.org/chromiumembedded/cef) (Chromium Embedded Framework) + C++20.  
Multi-process model: `LumenBrowser.exe` (browser) + `LumenBrowserSubprocess.exe` (renderer/GPU).  
UI toolbar rendered in a transparent child browser — enabling full CSS-level theming at runtime.

## Building

See [BUILDING.md](BUILDING.md) for full setup instructions.

Quick start (Windows, VS 2022):

```bat
cmake --preset windows-debug
cmake --build build/debug
build\debug\LumenBrowser.exe
```

## Theming

The theme engine accepts a `.lumentheme` file — a zip containing:

- `theme.json` — colors, geometry, effects, font settings
- `icons/` — SVG or PNG icon overrides
- `images/` — background images, splash screens

Import via **Settings → Appearance → Import .lumentheme**.

Custom assets placed in `themes-private/` (gitignored) are never committed to the repository.

## License

MIT
