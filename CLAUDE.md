# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

EACPExamples is a sandbox for exercising the [EACP](https://github.com/eyalamirmusic/eacp) and [Miro](https://github.com/eyalamirmusic/Miro) libraries end-to-end. It is not a library — it is a host that pulls both via CPM and builds native C++ apps that put their public APIs under real use, so we can find rough edges and feed fixes back upstream.

The current example is a native macOS/Windows app with an embedded WebView and a typed RPC bridge:

- `Apps/WebUIDemo/Main.cpp` — native entry point: `Window` + `WebView` + `WebViewBridge`.
- `Apps/WebUIDemo/Types.h` + `Commands.cpp` — request/response structs reflected with `MIRO_REFLECT`, plus a `MIRO_EXPORT_COMMAND`-registered handler.
- `Apps/WebUIDemo/web/` — Vite + TypeScript frontend bundled into the app via `eacp_webview_add_vite`. The `backend.greet(...)` function on the JS side is generated from the C++ schema by `miro_add_type_export` + `eacp_webview_generate_backend`.

## Build Commands

```bash
# Configure and build (always pass -DEACP_UNITY_BUILD=OFF for working LSP)
cmake -G Ninja -B build -DCMAKE_BUILD_TYPE=Debug -DEACP_UNITY_BUILD=OFF
cmake --build build

# Build a specific app
cmake --build build --target WebUIDemo
```

Output:
- `build/Apps/WebUIDemo/WebUIDemo.app` (macOS bundle)

### Local checkouts of EACP / Miro

EACP and Miro are fetched from GitHub `main` via CPM by default (see `CMake/FindEACP.cmake`, `CMake/FindMiro.cmake`). When co-developing this repo against local checkouts of either library, override the CPM source at configure time:

```bash
cmake -G Ninja -B build -DCMAKE_BUILD_TYPE=Debug -DEACP_UNITY_BUILD=OFF \
      -DCPM_EACP_SOURCE=$HOME/Code/eacp \
      -DCPM_Miro_SOURCE=$HOME/Code/Miro
```

Use `$HOME` (not `~`). CMake does not expand `~`, and shell tilde expansion is suppressed inside quotes — `-DCPM_Miro_SOURCE="~/Code/Miro"` will silently configure against a non-existent path and fail later with errors like `Unknown CMake command "miro_add_type_export"`.

## Architecture

```
EACPExamples/
  Apps/                  Example applications (one subdir per app)
    WebUIDemo/           Native app + Vite WebUI + Miro RPC schema
      web/               TypeScript frontend, bundled into the app
  CMake/                 CPM.cmake + Find modules + Warnings
  CMakeLists.txt         Root: pulls EACP + Miro, adds Apps/
```

The root `CMakeLists.txt` does the bare minimum: enables `OBJCXX` on Apple, calls `find_package(EACP REQUIRED)` and `find_package(Miro REQUIRED)` (which delegate to `CPMAddPackage` inside `CMake/FindEACP.cmake` / `FindMiro.cmake`), then `add_subdirectory(Apps)`. EACP's CMake helpers (`set_default_target_setting`, `eacp_webview_add_vite`, `eacp_webview_generate_backend`, `miro_add_type_export`) become available transitively once those packages are loaded.

New apps go under `Apps/<Name>/` with their own `CMakeLists.txt` and a top-level `add_subdirectory` call in `Apps/CMakeLists.txt`.

### Adding an RPC command

1. Define request/response structs in a header with `MIRO_REFLECT(...)` listing every field.
2. In a `.cpp` listed under `miro_add_type_export(SOURCES ...)`, define the handler and register it with `MIRO_EXPORT_COMMAND(name)`.
3. The codegen produces `web/src/generated/{schema.ts, backend.ts, ...}` so the TypeScript side gets typed `backend.<name>(...)` calls automatically.
4. Add the new `.cpp` to the `add_executable(...)` source list and to `miro_add_type_export(SOURCES ...)` so the schema is regenerated when it changes.

## Known upstream friction

Issues found while wiring this repo up — candidates to fix in EACP/Miro:

- **`Warnings.cmake` collision via `CMAKE_MODULE_PATH`.** Both Miro and EACP `include(Warnings)` from their own `CMake/`. Because the host project's `CMAKE_MODULE_PATH` is searched first, a host `CMake/Warnings.cmake` shadows them and breaks Miro's `miro_warnings` target. Worked around here by naming ours `ExamplesWarnings.cmake`. Upstream could prefix module names (`MiroWarnings.cmake`, `EacpWarnings.cmake`) so they're not in a global namespace.
- **`MIRO_EXPORT_COMMAND` rejects by-value request parameters with a cryptic error.** Only `R(*)(const A&)` and `R(*)()` have `CommandSignature` specializations, but a by-value handler (`R(*)(A)`) fails with `implicit instantiation of undefined template 'CommandSignature<...>'` and a useless `Req = int` cascade — not a clear "use `const A&`". Upstream could either accept by-value handlers or `static_assert` with a readable message.
- **Configure-time vite build needs build-time codegen output.** `eacp_webview_add_vite` runs `npm run build` via `execute_process` during configure. That build imports `./schema.backend` (and `./schema`), which Miro's `miro_add_type_export` only writes as a `POST_BUILD` step on a build-time tool — so a fresh checkout cannot configure cleanly. EACP's own `Apps/WebViewEmbed` masks this by committing the generated TS files. Worked around here by committing hand-written stubs of `schema.ts` and `schema.backend.ts`. Possible upstream fixes: defer the initial vite build to build time, or add a configure-time stub-emitting mode to `miro_add_type_export`.

## Code Style

- Modern C++20, RAII everywhere.
- Use `auto` for variables whenever possible. Use explicit return types for functions and member functions.
- Don't write comments unless absolutely needed — prefer self-documenting names.
- struct/class members go last, below methods.
- No `m_` or `_` prefixes. Use `xToUse` for input variables that shadow members.
- Allman braces, 4-space indent, 85-column limit, left-aligned pointers — enforced by `.clang-format` / `.clang-tidy` (copied from EACP).
- Always run clang-format on edited source files.
