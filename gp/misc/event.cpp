#include "event.hpp"

#include <stdexcept>

namespace gp::misc {
bool Event::MouseMoveData::left_is_down() const { return mouse_button_mask & MouseButtonMask::Left; }

bool Event::MouseMoveData::middle_is_down() const { return mouse_button_mask & MouseButtonMask::Middle; }

bool Event::MouseMoveData::right_is_down() const { return mouse_button_mask & MouseButtonMask::Right; }

Event::Event(const Type type, const std::uint64_t timestamp)
    : type_{type}
    , timestamp_{timestamp} {
  switch (type) {
  case Type::Init:
    data_.emplace<InitData>();
    break;
  case Type::Resize:
    data_.emplace<ResizeData>();
    break;
  case Type::MouseButton:
    data_.emplace<MouseButtonData>();
    break;
  case Type::MouseMove:
    data_.emplace<MouseMoveData>();
    break;
  case Type::MouseScroll:
    data_.emplace<MouseScrollData>();
    break;
  case Type::Key:
    data_.emplace<KeyData>();
    break;
  case Type::DragDrop:
    data_.emplace<DragDropData>();
    break;
  default:
    break;
  }
}

Event::Type Event::type() const { return type_; }

std::uint64_t Event::timestamp() const { return timestamp_; }

Event::InitData &Event::init() {
  if (type() != Type::Init) {
    throw std::runtime_error("Wrong data access");
  }
  return std::get<InitData>(data_);
}

const Event::InitData &Event::init() const {
  if (type() != Type::Init) {
    throw std::runtime_error("Wrong data access");
  }
  return std::get<InitData>(data_);
}

Event::ResizeData &Event::resize() {
  if (type() != Type::Resize) {
    throw std::runtime_error("Wrong data access");
  }
  return std::get<ResizeData>(data_);
}

const Event::ResizeData &Event::resize() const {
  if (type() != Type::Resize) {
    throw std::runtime_error("Wrong data access");
  }
  return std::get<ResizeData>(data_);
}

Event::MouseButtonData &Event::mouse_button() {
  if (type() != Type::MouseButton) {
    throw std::runtime_error("Wrong data access");
  }
  return std::get<MouseButtonData>(data_);
}

const Event::MouseButtonData &Event::mouse_button() const {
  if (type() != Type::MouseButton) {
    throw std::runtime_error("Wrong data access");
  }
  return std::get<MouseButtonData>(data_);
}

Event::MouseMoveData &Event::mouse_move() {
  if (type() != Type::MouseMove) {
    throw std::runtime_error("Wrong data access");
  }
  return std::get<MouseMoveData>(data_);
}

const Event::MouseMoveData &Event::mouse_move() const {
  if (type() != Type::MouseMove) {
    throw std::runtime_error("Wrong data access");
  }
  return std::get<MouseMoveData>(data_);
}

Event::MouseScrollData &Event::mouse_scroll() {
  if (type() != Type::MouseScroll) {
    throw std::runtime_error("Wrong data access");
  }
  return std::get<MouseScrollData>(data_);
}

const Event::MouseScrollData &Event::mouse_scroll() const {
  if (type() != Type::MouseScroll) {
    throw std::runtime_error("Wrong data access");
  }
  return std::get<MouseScrollData>(data_);
}

Event::KeyData &Event::key() {
  if (type() != Type::Key) {
    throw std::runtime_error("Wrong data access");
  }
  return std::get<KeyData>(data_);
}

const Event::KeyData &Event::key() const {
  if (type() != Type::Key) {
    throw std::runtime_error("Wrong data access");
  }
  return std::get<KeyData>(data_);
}

Event::DragDropData &Event::drag_drop() {
  if (type() != Type::DragDrop) {
    throw std::runtime_error("Wrong data access");
  }
  return std::get<DragDropData>(data_);
}

const Event::DragDropData &Event::drag_drop() const {
  if (type() != Type::DragDrop) {
    throw std::runtime_error("Wrong data access");
  }
  return std::get<DragDropData>(data_);
}
} // namespace gp::misc
