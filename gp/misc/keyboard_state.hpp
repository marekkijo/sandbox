#pragma once

#include <gp/misc/event.hpp>

#include <array>
#include <cstdint>

namespace gp::misc {
class KeyboardState {
public:
  KeyboardState() = default;
  ~KeyboardState() = default;

  KeyboardState(const KeyboardState &) = delete;
  KeyboardState &operator=(const KeyboardState &) = delete;
  KeyboardState(KeyboardState &&) = delete;
  KeyboardState &operator=(KeyboardState &&) = delete;

  [[nodiscard]] bool is_down(const Event::ScanCode scan_code) const;
  void process_key_event(const Event::KeyData &key_data);

private:
  std::array<bool, static_cast<std::size_t>(Event::ScanCode::EnumCount)> keys_{};
};
} // namespace gp::misc
