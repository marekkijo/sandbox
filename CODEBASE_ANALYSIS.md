# Sandbox Project — Structural Analysis & Refactoring Proposal

> Verified: 2026-04-29  
> Branch: `main` (all findings confirmed against actual source; open PRs excluded)  
> Scope: full static review of C++ source tree  
> Purpose: authoritative input for ticket creation and implementation work

---

## Table of Contents

1. [🔴 Bugs](#1-bugs)
2. [🟠 Design Risks](#2-design-risks)
3. [🔵 Structural / Maintainability](#3-structural--maintainability)
4. [Suggested Ticket List](#4-suggested-ticket-list)
5. [Quick-start prompt for Copilot](#5-quick-start-prompt-for-copilot)

---

## 1. Bugs

### 1.1 `split_by()` uses absolute find-position as substring length

| Field | Detail |
|---|---|
| **File** | `gp/utils/utils.cpp` |
| **Severity** | 🔴 Medium — wrong output for any string with two or more non-leading delimiters |
| **Call sites** | `streaming/streaming_signaling_server/client.cpp` (URL path and label parsing) |

**Root cause**  
`std::string::substr(pos, len)` takes a *start position* and a *length*.
The implementation passes `pos` (the absolute index returned by `find`) as the length instead of `pos - ppos`:

```cpp
// current — WRONG
tokens.emplace_back(src.substr(ppos, pos));
//                                    ^^^ absolute index, not length

// should be
tokens.emplace_back(src.substr(ppos, pos - ppos));
```

A second issue: the final token uses `src.length() - 1` as the length, which silently drops the last character when `ppos == 0`:

```cpp
// current — WRONG (off-by-one when ppos == 0)
tokens.emplace_back(src.substr(ppos, src.length() - 1));

// should be (no explicit length → take everything from ppos to end)
tokens.emplace_back(src.substr(ppos));
```

**Current exposure**  
Both call sites (`split_by(path, "/")` then `split_by(part, ":")`) happen to pass strings with a single delimiter occurrence, so the wrong `pos` value accidentally equals `pos - ppos` (because `ppos == 0` at the only match). The bugs are latent but will surface immediately if any WebSocket path ever gains more than one `/` or `:` segment.

**Fix**

```cpp
std::vector<std::string> split_by(const std::string &src, const std::string &delimiter) {
    auto tokens = std::vector<std::string>{};
    auto ppos = std::size_t{0u};
    std::size_t pos;
    while ((pos = src.find(delimiter, ppos)) != std::string::npos) {
        tokens.emplace_back(src.substr(ppos, pos - ppos));
        ppos = pos + delimiter.size();
    }
    tokens.emplace_back(src.substr(ppos));
    return tokens;
}
```

---

### 1.2 Wrong error message in `Decoder::incoming_data()`

| Field | Detail |
|---|---|
| **File** | `streaming/streaming_common/decoder.cpp` |
| **Severity** | 🔴 Low — misleading diagnostic when the decoder is used before `init()` |

**Root cause**  
The guard throws with the message `"Decoder is already initialized"` in the exact opposite situation — when the decoder has *not* yet been initialized (`rgb_frame_` is null):

```cpp
// current — wrong message
if (!async && !rgb_frame_) {
    throw std::runtime_error{"Decoder is already initialized"};
    //                                 ^^^^^^^^^^^ says "already" but means "not yet"
}
```

**Fix**

```cpp
if (!async && !rgb_frame_) {
    throw std::runtime_error{"Decoder is not initialized"};
}
```

---

### 1.3 Dead `continue` in `Encoder::encode_frame()` drain loop

| Field | Detail |
|---|---|
| **File** | `streaming/streaming_common/encoder.cpp` |
| **Severity** | 🔴 Low — dead code; obscures intent |

**Root cause**  
The loop condition is `while (result == 0)`. When `avcodec_receive_packet` returns `AVERROR(EAGAIN)`, `result` is no longer `0`, so `continue` simply re-evaluates the `while` condition and exits — it does not retry the receive call as the author likely intended.

```cpp
// current
while (result == 0) {
    result = avcodec_receive_packet(context_.get(), packet_.get());
    if (result == 0) {
        // process packet …
    } else if (result == AVERROR(EAGAIN)) {
        continue;   // ← dead: loop exits immediately because result != 0
    } else if (result == AVERROR_EOF) {
        // flush …
    } else {
        throw …;
    }
}
```

**Fix** — replace `continue` with `break` (or remove the branch entirely and let the loop exit naturally):

```cpp
} else if (result == AVERROR(EAGAIN)) {
    break;  // need more input; try again on the next encode() call
}
```

---

## 2. Design Risks

### 2.1 `Server` registers WebSocket callbacks with a raw `this` pointer

| Field | Detail |
|---|---|
| **File** | `streaming/streaming_signaling_server/server.cpp` |
| **Severity** | 🟠 Medium — undefined behaviour if a callback fires after `Server` is destroyed |

**Root cause**  
Both `Server::Server()` and `Server::init_client()` bind callbacks with `std::bind(&Server::…, this, …)`. `Server` does not inherit from `std::enable_shared_from_this`, so there is no way to produce a `weak_ptr` that goes stale safely. If any queued WebSocket callback fires after the `Server` object is destroyed, `this` is dangling.

In the current `main()` the `Server` lives for the entire process lifetime, so the bug is not triggered in practice — but it is one refactor away from a crash.

**Fix** — manage `Server` via `shared_ptr` and use `weak_from_this()` in callbacks, mirroring the pattern already used in `Receiver` and `Streamer`:

```cpp
class Server : public std::enable_shared_from_this<Server> { … };

// In a separate start() method (NOT the constructor):
std::weak_ptr<Server> weak_self = weak_from_this();
server_.onClient([weak_self](auto ws) {
    if (auto self = weak_self.lock()) self->on_client(std::move(ws));
});
```

> **Note**: `start()` must be called *after* the `Server` object is placed inside a `shared_ptr`. Do **not** register callbacks from the constructor.

---

## 3. Structural / Maintainability

### 3.1 CMake `file(GLOB …)` — `CONFIGURE_DEPENDS` is the variable name, not the flag

| Field | Detail |
|---|---|
| **Files** | All module-level `CMakeLists.txt` files (`ai/`, `loaders/`, `streaming/`, `wolf/`, `gp/`) |
| **Severity** | 🔵 Low — CONFIGURE_DEPENDS re-scan is silently never applied |

**Root cause**  
CMake's `file(GLOB <variable> [CONFIGURE_DEPENDS] <patterns>)` expects the variable name *before* the `CONFIGURE_DEPENDS` keyword. The codebase has the arguments in the wrong order:

```cmake
# current — CONFIGURE_DEPENDS becomes the variable name; SRC_FILES is a glob pattern
file(GLOB CONFIGURE_DEPENDS SRC_FILES *.cpp *.hpp)

# correct
file(GLOB SRC_FILES CONFIGURE_DEPENDS *.cpp *.hpp)
```

Because `CONFIGURE_DEPENDS` ends up as the variable name, the flag is never applied and CMake does not re-scan when source files are added or removed.

**Fix** — swap the argument order in every occurrence:

```cmake
file(GLOB SRC_FILES CONFIGURE_DEPENDS *.cpp *.hpp)
file(GLOB_RECURSE FFMPEG_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/*")
# … and so on for every file(GLOB …) call
```

---

### 3.2 `printf()` used for all runtime diagnostics

| Field | Detail |
|---|---|
| **Files** | `streaming/` — `server.cpp`, `receiver.cpp`, `streamer.cpp`, `decoder.cpp` |
| **Severity** | 🔵 Low — no severity levels, no structured output, cannot be disabled |

All diagnostic output in the streaming subsystem goes through raw `printf()`. There is no log-level filtering, no timestamps, and no way to suppress output in tests or quiet mode.

**Recommendation** — adopt a lightweight logging header (e.g., `spdlog`, or a thin project-owned wrapper) so that callers can set a minimum level at startup.

---

## 4. Suggested Ticket List

| # | Title | Severity | File(s) |
|---|---|---|---|
| T-01 | Fix `split_by()` — use `pos - ppos` as length and `substr(ppos)` for final token | 🔴 Medium | `gp/utils/utils.cpp` |
| T-02 | Fix wrong error message in `Decoder::incoming_data()` ("already" → "not") | 🔴 Low | `streaming/streaming_common/decoder.cpp` |
| T-03 | Replace dead `continue` with `break` in `Encoder::encode_frame()` | 🔴 Low | `streaming/streaming_common/encoder.cpp` |
| T-04 | Make `Server` safe against post-destruction callbacks (`enable_shared_from_this` + `weak_from_this`) | 🟠 Medium | `streaming/streaming_signaling_server/server.cpp` |
| T-05 | Fix `file(GLOB …)` argument order to apply `CONFIGURE_DEPENDS` flag | 🔵 Low | all module `CMakeLists.txt` |
| T-06 | Replace `printf()` with structured logging in the streaming subsystem | 🔵 Low | `streaming/` |

---

## 5. Quick-start prompt for Copilot

Paste this block at the top of your Copilot chat together with this file:

```
I have a C++ codebase analysis document (CODEBASE_ANALYSIS.md).
Every finding has been manually verified against the current main branch.
Please:
1. For each item in the Suggested Ticket List, open the referenced source file
   and confirm whether the issue still exists exactly as described.
2. For confirmed issues, draft a GitHub issue (title, body, acceptance criteria, labels).
3. For items T-01 through T-04, propose a minimal, self-contained code fix.
Start with T-01 (split_by fix) as it is the highest-risk latent bug.
```
