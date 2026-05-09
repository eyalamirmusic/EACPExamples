# EACPExamples

A sandbox for exercising [EACP](https://github.com/eyalamirmusic/eacp) and
[Miro](https://github.com/eyalamirmusic/Miro) end-to-end. The goal is to put
both libraries under real use in a native C++ app and surface anything that
isn't ergonomic enough to feed back upstream.

## What's here

- `Apps/WebUIDemo/` — native macOS/Windows app with an embedded EACP `WebView`
  and a Vite + TypeScript frontend, talking to native code via a Miro-typed
  RPC bridge.

## Building

CMake 3.31+, C++20 toolchain, Node (for the Vite frontend). EACP and Miro are
fetched automatically via CPM at configure time.

```bash
cmake -G Ninja -B build -DCMAKE_BUILD_TYPE=Debug -DEACP_UNITY_BUILD=OFF
cmake --build build --target WebUIDemo
```

Result: `build/Apps/WebUIDemo/WebUIDemo.app` (macOS).

To work against local checkouts of EACP or Miro:

```bash
cmake -G Ninja -B build -DCMAKE_BUILD_TYPE=Debug -DEACP_UNITY_BUILD=OFF \
      -DCPM_EACP_SOURCE=$HOME/Code/eacp \
      -DCPM_Miro_SOURCE=$HOME/Code/Miro
```

## Layout

```
Apps/         Example applications
CMake/        CPM.cmake + Find modules + Warnings
CMakeLists.txt
```

## License

MIT — see [LICENSE](LICENSE).
