#include "keyboard_state.hpp"

namespace gp::misc {
bool KeyboardState::is_down(const Event::ScanCode scan_code) const {
  return keys_[static_cast<std::size_t>(scan_code)];
}

void KeyboardState::process_key_event(const Event::KeyData &key_data) {
  keys_[static_cast<std::size_t>(key_data.scan_code)] = key_data.action == Event::Action::Pressed;
}
} // namespace gp::misc
