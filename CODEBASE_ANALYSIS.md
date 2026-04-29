# Codebase Analysis – `marekkijo/sandbox`

> Generated: 2026-04-29  
> Scope: full static analysis of C++ source tree  
> Purpose: feed into Copilot / ticket-creation workflow

---

## Table of Contents

1. [🔴 Critical Bugs](#1-critical-bugs)
2. [🔴 Thread Safety / Undefined Behaviour](#2-thread-safety--undefined-behaviour)
3. [🟠 Resource Safety & Input Validation](#3-resource-safety--input-validation)
4. [🔵 Structural / Maintainability](#4-structural--maintainability)
5. [Suggested Ticket List](#5-suggested-ticket-list)

---

## 1. Critical Bugs

### 1.1 Wrong operator in `on_segment()` — always returns wrong result

| Field | Detail |
|---|---|
| **File** | *(geometry / computational-geometry utility, search for `on_segment`)* |
| **Severity** | 🔴 Critical – logic is silently wrong |
| **Root cause** | The second call uses `std::max` where `std::min` is required. The condition `q == max(p, r)` is only true when `q` equals the larger endpoint, so collinear points between the endpoints are never recognised as "on the segment". |

**Broken pattern**
```cpp
// Both branches use std::max – the second must be std::min
return (q.x <= std::max(p.x, r.x) && q.x >= std::max(p.x, r.x) && ...);
//                                               ^^^ should be std::min
```

**Fix**
```cpp
return (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
        q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y));
```

---

### 1.2 Typo `vipe_data()` → `wipe_data()` in `VoxelScene`

| Field | Detail |
|---|---|
| **File** | `wolf/` – `VoxelScene` class |
| **Severity** | 🔴 Critical – function is never called; data is never cleared |
| **Root cause** | Method declared/defined as `vipe_data()` but every call site spells it `wipe_data()`, so the intended cleanup is silently skipped. |

**Fix** — rename the declaration, definition, and any stray call sites:
```cpp
// wolf/wolf_common/wolf_scene.hpp  (and .cpp)
void wipe_data();   // was: vipe_data()
```

---

### 1.3 Unbounded tail-recursion in `Decoder::upload()` on malformed input

| Field | Detail |
|---|---|
| **File** | `streaming/streaming_common/decoder.cpp` |
| **Severity** | 🔴 Critical – stack overflow on malformed/fuzz input |
| **Root cause** | `upload()` calls itself recursively without a depth limit. A stream with repeated invalid frames will exhaust the stack. |

**Fix** – convert to an iterative loop:
```cpp
void Decoder::upload(const std::vector<std::byte>& data) {
    std::size_t offset = 0;
    while (offset < data.size()) {
        // process next chunk, advance offset
    }
}
```

---

### 1.4 Unsigned integer underflow in `Decoder::incoming_data()`

| Field | Detail |
|---|---|
| **File** | `streaming/streaming_common/decoder.cpp` |
| **Severity** | 🔴 Critical – wraps to a huge positive value → out-of-bounds read |
| **Root cause** | Code subtracts `NULL_PADDING.size()` from `buffer_.size()` without first checking that `buffer_.size() >= NULL_PADDING.size()`. Both are `std::size_t` (unsigned), so underflow wraps. |

**Fix**
```cpp
if (buffer_.size() < NULL_PADDING.size()) return;
const auto payload_size = buffer_.size() - NULL_PADDING.size();
```

---

### 1.5 Use-after-free / stale value in `Decoder::decode()`

| Field | Detail |
|---|---|
| **File** | `streaming/streaming_common/decoder.cpp` |
| **Severity** | 🔴 Critical – reads from a pointer that `destroy()` just nullified |
| **Root cause** | `context_->frame_num` is read after `destroy()` sets `context_` to `nullptr` (or frees the pointed-to object), producing a use-after-free or read of indeterminate value. |

**Fix** – capture the value *before* calling `destroy()`:
```cpp
const auto frame_num = context_->frame_num;  // capture first
destroy();                                    // then destroy
log(frame_num);                               // use captured value
```

---

## 2. Thread Safety / Undefined Behaviour

### 2.1 Un-guarded shared state in signalling `Server`

| Field | Detail |
|---|---|
| **File** | `streaming/` – signalling server implementation |
| **Severity** | 🔴 Critical – data race → UB |
| **Root cause** | `clients_`, `streamers_`, `receivers_`, and `temporary_store_` are accessed from multiple WebSocket callback threads without any synchronisation primitive. |

**Fix** – protect every access with a `std::mutex`:
```cpp
// In the class
std::mutex mtx_;
std::unordered_map<Id, Client> clients_;   // guarded by mtx_

// In every callback
std::lock_guard<std::mutex> lock(mtx_);
clients_[id] = ...;
```

---

### 2.2 `std::bind(this, …)` callbacks in `Receiver` / `Streamer` constructors

| Field | Detail |
|---|---|
| **File** | `streaming/streaming_receiver/receiver.cpp`, `streaming/streaming_streamer/streamer.cpp` |
| **Severity** | 🔴 Critical – callbacks fire after object destruction → dangling `this` |
| **Root cause** | Callbacks are registered with raw `this` (via `std::bind` or capturing lambdas) inside the constructor, before the object is owned by a `shared_ptr`. If a callback fires after the object is destroyed the pointer is dangling. |

**Fix** – use `weak_from_this()` and register callbacks in `connect()`/`start()`, not the constructor:
```cpp
// receiver.hpp – inherit from enable_shared_from_this
class Receiver : public std::enable_shared_from_this<Receiver> { ... };

// receiver.cpp
void Receiver::connect() {
    auto weak = weak_from_this();                // valid: object already in shared_ptr
    web_socket_->onMessage([weak](auto msg) {
        if (auto self = weak.lock()) self->on_message(msg);
    });
    web_socket_->open(connection_url_);
}
```

> **Note**: `connect()` / `start()` must be called *after* the object is placed in a `shared_ptr`. Do **not** call them in the constructor.

---

## 3. Resource Safety & Input Validation

### 3.1 Raw FFmpeg pointer lifetimes (leaks on exception)

| Field | Detail |
|---|---|
| **Files** | `streaming/streaming_common/decoder.cpp`, `streaming/streaming_common/encoder.cpp` (and related) |
| **Severity** | 🟠 High – resource leak on any early return or exception |
| **Root cause** | `AVCodecContext*`, `AVPacket*`, `AVFrame*`, and `SwsContext*` are managed manually. Any exception or early `return` before the matching `av_free*` call leaks the resource. |

**Fix** – use the RAII wrappers already available in `gp/ffmpeg/ffmpeg.hpp` (`namespace gp::ffmpeg`):

| Raw type | RAII wrapper |
|---|---|
| `AVCodecContext*` | `gp::ffmpeg::UniqueAVCodecContext` |
| `AVCodecParserContext*` | `gp::ffmpeg::UniqueAVCodecParserContext` |
| `AVPacket*` | `gp::ffmpeg::UniqueAVPacket` |
| `AVFrame*` | `gp::ffmpeg::UniqueAVFrame` |
| `SwsContext*` | `gp::ffmpeg::UniqueSwsContext` |

Once RAII wrappers are used the manual `destroy()` method can be removed entirely.

---

### 3.2 Division by zero in `WolfScene` constructor (`number_of_rays == 0`)

| Field | Detail |
|---|---|
| **File** | `wolf/wolf_common/wolf_scene.cpp` |
| **Severity** | 🟠 High – undefined behaviour |
| **Root cause** | No guard against `number_of_rays == 0`; downstream code divides by it. |

**Fix**
```cpp
WolfScene::WolfScene(int number_of_rays, ...) {
    if (number_of_rays == 0)
        throw std::invalid_argument("number_of_rays must be > 0");
    ...
}
```

---

### 3.3 Out-of-bounds index in `NeuralNetwork::set_input()`

| Field | Detail |
|---|---|
| **File** | `ai/ai_common/neural_network.cpp` |
| **Severity** | 🟠 High – buffer overrun |
| **Root cause** | `set_input(index, value)` does not validate `index` before writing into the input layer's neuron array. |

**Fix**
```cpp
void NeuralNetwork::set_input(std::size_t index, float value) {
    if (index >= input_layer_.size())
        throw std::out_of_range("NeuralNetwork::set_input: index out of range");
    input_layer_[index] = value;
}
```

---

### 3.4 Silent failure in `load_txt_file()`

| Field | Detail |
|---|---|
| **File** | utility / file-loading helper |
| **Severity** | 🟠 Medium – caller receives empty/garbage data with no indication of failure |
| **Root cause** | `std::ifstream` open failure is not checked; the function returns whatever an un-opened stream produces. |

**Fix**
```cpp
std::string load_txt_file(const std::filesystem::path& path) {
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("load_txt_file: cannot open " + path.string());
    return { std::istreambuf_iterator<char>(file), {} };
}
```

---

### 3.5 `RawMap::BlockType::operator[]` returns reference to local static

| Field | Detail |
|---|---|
| **File** | `wolf/` – `RawMap` |
| **Severity** | 🟠 High – dangling reference / UB on out-of-bounds access |
| **Root cause** | On out-of-range index the operator returns a reference to a `static` local variable, which is shared across all calls and not the caller's data. |

**Fix** – throw instead:
```cpp
BlockType& operator[](std::size_t idx) {
    if (idx >= data_.size())
        throw std::out_of_range("RawMap::BlockType: index out of range");
    return data_[idx];
}
```

---

### 3.6 Model loaders silently return empty scene on failure

| Field | Detail |
|---|---|
| **Files** | `loaders/loaders_assimp/model.cpp`, `loaders/loaders_cgltf/model.cpp` |
| **Severity** | 🟠 Medium – caller has no way to distinguish "empty file" from "load error" |
| **Root cause** | When Assimp / cgltf report an error the loaders return a default-constructed `ModelScene` without signalling the error. |

**Fix** – throw `std::runtime_error` on failure:
```cpp
ModelScene Model::load(const std::filesystem::path& path) {
    // assimp variant
    const aiScene* scene = importer.ReadFile(path.string(), flags);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        throw std::runtime_error("Assimp failed to load: " + importer.GetErrorString());
    ...
}
```

---

### 3.7 Unnamed `std::lock_guard` in `encode_scene.cpp` — zero protection

| Field | Detail |
|---|---|
| **File** | `streaming/streaming_encode_decode/encode_scene.cpp` (or similar) |
| **Severity** | 🟠 High – the mutex is acquired and *immediately* released; the critical section is not protected |
| **Root cause** | `std::lock_guard<std::mutex>{mtx_};` — a temporary with no name is destroyed at the semicolon. |

**Fix** – give it a name:
```cpp
std::lock_guard<std::mutex> lock(mtx_);   // stays alive for the scope
```

---

## 4. Structural / Maintainability

### 4.1 Inconsistent namespaces in `loaders` modules

| Field | Detail |
|---|---|
| **Files** | `loaders/loaders_assimp/`, `loaders/loaders_cgltf/` |
| **Severity** | 🔵 Low – style / collision risk |

All public classes (`Model`, `ModelScene`) should be wrapped in `namespace loaders {}` consistent with `namespace ai {}`, `namespace streaming {}`, and `namespace wolf {}`.

---

### 4.2 24-branch if-chain in `get_orientation_matrix()`

| Field | Detail |
|---|---|
| **File** | search for `get_orientation_matrix` |
| **Severity** | 🔵 Low – maintainability |

Replace with a `static const std::unordered_map<int, glm::mat4>` keyed on `up * 6 + front`. Throws `std::invalid_argument` for unknown combinations.

---

### 4.3 CMake `file(GLOB …)` without `CONFIGURE_DEPENDS`

| Field | Detail |
|---|---|
| **Files** | Multiple `CMakeLists.txt` across the tree |
| **Severity** | 🔵 Low – stale builds after adding/removing source files |

**Fix** – add `CONFIGURE_DEPENDS` to every `file(GLOB …)` call:
```cmake
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")
```

---

### 4.4 `#define CGLTF_IMPLEMENTATION` not isolated in a dedicated TU

| Field | Detail |
|---|---|
| **Files** | `loaders/loaders_cgltf/` |
| **Severity** | 🔵 Low – violates single-definition rule if the header is included in multiple TUs |

**Fix** – create `cgltf_impl.cpp` containing only:
```cpp
#define CGLTF_IMPLEMENTATION
#include "cgltf.h"
```
Remove the `#define` from all other files.

---

## 5. Suggested Ticket List

| # | Title | Severity | Files |
|---|---|---|---|
| T-01 | Fix wrong `std::max` → `std::min` in `on_segment()` | 🔴 Critical | geometry utility |
| T-02 | Rename `vipe_data()` → `wipe_data()` in `VoxelScene` | 🔴 Critical | `wolf/wolf_common/` |
| T-03 | Replace tail-recursion with loop in `Decoder::upload()` | 🔴 Critical | `streaming/streaming_common/decoder.cpp` |
| T-04 | Guard unsigned underflow in `Decoder::incoming_data()` | 🔴 Critical | `streaming/streaming_common/decoder.cpp` |
| T-05 | Fix use-after-free in `Decoder::decode()` (capture before destroy) | 🔴 Critical | `streaming/streaming_common/decoder.cpp` |
| T-06 | Add mutex to signalling `Server` shared state | 🔴 Critical | `streaming/` server |
| T-07 | Replace raw-`this` callbacks with `weak_from_this()` in `Receiver`/`Streamer` | 🔴 Critical | `streaming/streaming_receiver/`, `streaming/streaming_streamer/` |
| T-08 | Replace raw FFmpeg pointers with RAII wrappers | 🟠 High | `streaming/streaming_common/` |
| T-09 | Guard `number_of_rays == 0` in `WolfScene` | 🟠 High | `wolf/wolf_common/wolf_scene.cpp` |
| T-10 | Validate index in `NeuralNetwork::set_input()` | 🟠 High | `ai/ai_common/neural_network.cpp` |
| T-11 | Throw on file-open failure in `load_txt_file()` | 🟠 Medium | utility |
| T-12 | Fix `RawMap::BlockType::operator[]` returning ref-to-static | 🟠 High | `wolf/` |
| T-13 | Throw on model-load failure (assimp + cgltf) | 🟠 Medium | `loaders/loaders_assimp/`, `loaders/loaders_cgltf/` |
| T-14 | Fix unnamed `std::lock_guard` in `encode_scene.cpp` | 🟠 High | `streaming/streaming_encode_decode/` |
| T-15 | Wrap loaders classes in `namespace loaders` | 🔵 Low | `loaders/` |
| T-16 | Replace `get_orientation_matrix()` if-chain with map | 🔵 Low | orientation utility |
| T-17 | Add `CONFIGURE_DEPENDS` to all CMake `file(GLOB …)` calls | 🔵 Low | `CMakeLists.txt` files |
| T-18 | Isolate `#define CGLTF_IMPLEMENTATION` in dedicated TU | 🔵 Low | `loaders/loaders_cgltf/` |

---

## Quick-start prompt for Copilot

Paste this block at the top of your Copilot chat together with this file:

```
I have a C++ codebase analysis document (CODEBASE_ANALYSIS.md).
Please:
1. For each item in the Suggested Ticket List, open the referenced source file
   and confirm whether the issue still exists.
2. For confirmed issues, draft a GitHub issue (title, body, labels).
3. For Critical issues, propose a minimal, self-contained code fix.
Start with the 🔴 Critical items (T-01 through T-07).
```
