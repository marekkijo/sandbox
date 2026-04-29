# Copilot Instructions

## Build & Test

```bash
# Configure
cmake --preset=ninja .          # macOS/Linux with Ninja + clang-tidy
cmake --preset=vs2022 .         # Windows
cmake --preset=xcode .          # macOS with Xcode
cmake --preset=emscripten .     # WebAssembly

# Build
cmake --build --preset=ninja
cmake --build --preset=ninja_debug

# Run all tests (ninja preset only; tests excluded from Emscripten builds)
ctest --preset ninja
ctest --preset ninja_debug

# Run a single test by name
ctest --preset ninja -R sdl_tests
```

Clang-tidy is enabled automatically with the `ninja` preset (`CMAKE_CXX_CLANG_TIDY=clang-tidy`). `compile_commands.json` is copied to the source root after Ninja/Makefile builds.

Pre-commit hooks handle formatting — run `pre-commit run --all-files` to check. Formatting uses clang-format with LLVM base style, 120-column limit.

## Architecture

The repo is a **C++23 sandbox** of independent graphical/multimedia demo applications, all sharing a common `gp` library.

### Module layout

| Directory | Purpose |
|-----------|---------|
| `gp/` | **General Purpose library** — static lib linked by all modules. Contains SDL scene base classes, OpenGL helpers, FFmpeg RAII wrappers, GLM math utilities, JSON helpers, and `gp::utils`. |
| `ai/` | Neural-network demos (`ai_common` lib + `ai_pong` executable). |
| `loaders/` | Asset loaders: `loaders_assimp` (Assimp) and `loaders_cgltf` (cgltf), both expose a `Model`/`ModelScene` API under `namespace loaders`. |
| `streaming/` | WebRTC video streaming pipeline — signaling server, streamer (encoder side), receiver (decoder side), and an encode/decode test binary. **Excluded from Emscripten builds.** |
| `wolf/` | Wolfenstein-style raycaster demos (singlethread, multithread, voxelizer, geometry test, etc.). |
| `cmake/` | `ResourceUtils.cmake` and `TestsUtils.cmake` — custom CMake helpers. |

### `gp` library internals

- **Scene base classes** (`gp/sdl/`): `gp::sdl::Scene2D` and `gp::sdl::Scene3D`. Subclass one, override `loop(const misc::Event &event)`, call `init(width, height, title)` then `exec()`.
- **FFmpeg RAII** (`gp/ffmpeg/ffmpeg.hpp`): `UniqueAVCodecContext`, `UniqueAVPacket`, `UniqueAVFrame`, `UniqueSwsContext`, `UniqueAVCodecParserContext` — all in `namespace gp::ffmpeg`.
- **Working directory** (`gp/utils/utils.hpp`): Every executable calls `gp::utils::set_working_directory()` at the top of `main()` so that relative paths like `data/` and `shaders/` resolve from the binary's directory.

### CMake conventions

- Dependencies are managed by **vcpkg** (toolchain auto-applied via presets).
- Assets are wired with `add_target_resources(<target> RESOURCES <dir> ...)` from `ResourceUtils.cmake`. For Emscripten this embeds via `--preload-file`; for native builds it copies with a PRE_BUILD command.
- Tests live in `_tests_/` subdirectories and are registered with `add_test_target(<name> FILES ... DEPS ...)` from `TestsUtils.cmake` (links GTest/GMock, registers with CTest, sets IDE folder).
- `gp` and all test targets compile with `cxx_std_23`; the root project sets `cxx_std_17` as a floor.

## Commit Messages

Subject line format: `<issue-number>: <Short description>`

```
120: Add copilot-instructions.md
68: Create a final Wolf target with fast ray collisions (#71)
65: Wolf - Implement wall collisions (#66)
```

Body lines (when needed) follow the same prefix: `* <issue-number>: <detail>`.

## Key Conventions

### Namespaces
Each module has its own namespace: `gp::*`, `ai::*`, `streaming::*`, `wolf::*`, `loaders::*`.

### CLI argument pattern
Every executable uses the same pattern: a local `ProgramSetup` struct + a `process_args(argc, argv)` function using `boost::program_options`. Check `vm.count("help")` before accessing values.

### `shared_ptr` lifecycle for streaming
`streaming::Receiver` and `streaming::Streamer` inherit from `std::enable_shared_from_this`. WebSocket/DataChannel callbacks must be registered **after** the object is owned by a `shared_ptr` — call `connect()`/`start()` after construction, never in constructors. Callbacks hold `weak_from_this()` to avoid dangling references.

### Emscripten guards
Use `if(NOT EMSCRIPTEN)` in CMake to exclude desktop-only code (FFmpeg, libdatachannel, GTest, the entire `streaming` subtree). In C++ use `#ifdef __EMSCRIPTEN__` where necessary.
