# Dev Mode Bug

## Problem

1. **Toolbar always showed DevTools + Reload** regardless of whether the app was in development or production.  
   Root cause: `m_isDevelopmentMode` in `WebEngineWindow` was always initialized to `false` and never received the `devMode` value from `main.cpp`.

2. **Dev mode was never auto-detected** from build type. It required manually passing `--dev` flag, which meant a deployed Release build and a debug build behaved the same way unless the flag was explicitly set.

3. **Windows Defender alert** after logging was added — caused by the app spawning a visible console window (CMD/PowerShell) because `WIN32_EXECUTABLE` was commented out in `CMakeLists.txt`. A GUI app that opens a console window and does network operations triggers SmartScreen heuristics.

## Fixes Applied

### `backend/webengine/webengine_window.h`
- Added `bool devMode = false` parameter to `WebEngineWindow` constructor.

### `backend/webengine/webengine_window.cpp`
- Constructor initializer changed from `m_isDevelopmentMode(false)` to `m_isDevelopmentMode(devMode)`.
- Reload and DevTools toolbar actions wrapped in `if (m_isDevelopmentMode)` — only visible in dev mode. About button always visible.

### `backend/main.cpp`
- `devMode` now auto-detected via `#ifdef NDEBUG`:
  - **Debug build** → `devMode = true` (dev toolbar, logs active)
  - **Release build** → `devMode = false` (clean toolbar, logs silenced)
- `--dev` flag still works as a manual override.
- `devMode` is now passed to `WebEngineWindow window(port, devMode)`.
- Added `silentMessageHandler` + `qInstallMessageHandler` to suppress all Qt logging in production (no `--dev`).

## Expected Behavior

| Build Type              | Dev Mode      | Toolbar                   | Logging |
| ----------------------- | ------------- | ------------------------- | ------- |
| Debug                   | auto `true`   | Reload + DevTools + About | Active  |
| Release (deployed .exe) | auto `false`  | About only                | Silent  |
| Any + `--dev` flag      | forced `true` | Reload + DevTools + About | Active  |

## Files Changed
- `backend/webengine/webengine_window.h`
- `backend/webengine/webengine_window.cpp`
- `backend/main.cpp`
