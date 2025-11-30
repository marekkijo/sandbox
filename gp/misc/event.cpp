#include "event.hpp"

#include <stdexcept>

namespace gp::misc {
bool Event::MouseMoveData::left_is_down() const { return mouse_button_mask & MouseButtonMask::Left; }

bool Event::MouseMoveData::middle_is_down() const { return mouse_button_mask & MouseButtonMask::Middle; }

bool Event::MouseMoveData::right_is_down() const { return mouse_button_mask & MouseButtonMask::Right; }

Event::Event(const Type type, const std::uint64_t timestamp)
    : type_{type}
    , timestamp_{timestamp} {
  construct_complex_members();
}

Event::Event(const Event &other)
    : type_{other.type()}
    , timestamp_{other.timestamp()} {
  copy(other);
}

Event::Event(Event &&other) noexcept
    : type_{other.type()}
    , timestamp_{other.timestamp()} {
  move(std::move(other));
}

Event &Event::operator=(const Event &other) {
  destruct_complex_members();
  copy(other);
  return *this;
}

Event &Event::operator=(Event &&other) noexcept {
  destruct_complex_members();
  move(std::move(other));
  return *this;
}

Event::~Event() { destruct_complex_members(); }

Event::Type Event::type() const { return type_; }

std::uint64_t Event::timestamp() const { return timestamp_; }

Event::InitData &Event::init() {
  if (type() != Type::Init) {
    throw std::runtime_error("Wrong data access");
  }
  return init_;
}

const Event::InitData &Event::init() const {
  if (type() != Type::Init) {
    throw std::runtime_error("Wrong data access");
  }
  return init_;
}

Event::ResizeData &Event::resize() {
  if (type() != Type::Resize) {
    throw std::runtime_error("Wrong data access");
  }
  return resize_;
}

const Event::ResizeData &Event::resize() const {
  if (type() != Type::Resize) {
    throw std::runtime_error("Wrong data access");
  }
  return resize_;
}

Event::MouseButtonData &Event::mouse_button() {
  if (type() != Type::MouseButton) {
    throw std::runtime_error("Wrong data access");
  }
  return mouse_button_;
}

const Event::MouseButtonData &Event::mouse_button() const {
  if (type() != Type::MouseButton) {
    throw std::runtime_error("Wrong data access");
  }
  return mouse_button_;
}

Event::MouseMoveData &Event::mouse_move() {
  if (type() != Type::MouseMove) {
    throw std::runtime_error("Wrong data access");
  }
  return mouse_move_;
}

const Event::MouseMoveData &Event::mouse_move() const {
  if (type() != Type::MouseMove) {
    throw std::runtime_error("Wrong data access");
  }
  return mouse_move_;
}

Event::MouseScrollData &Event::mouse_scroll() {
  if (type() != Type::MouseScroll) {
    throw std::runtime_error("Wrong data access");
  }
  return mouse_scroll_;
}

const Event::MouseScrollData &Event::mouse_scroll() const {
  if (type() != Type::MouseScroll) {
    throw std::runtime_error("Wrong data access");
  }
  return mouse_scroll_;
}

Event::KeyData &Event::key() {
  if (type() != Type::Key) {
    throw std::runtime_error("Wrong data access");
  }
  return key_;
}

const Event::KeyData &Event::key() const {
  if (type() != Type::Key) {
    throw std::runtime_error("Wrong data access");
  }
  return key_;
}

Event::DragDropData &Event::drag_drop() {
  if (type() != Type::DragDrop) {
    throw std::runtime_error("Wrong data access");
  }
  return drag_drop_;
}

const Event::DragDropData &Event::drag_drop() const {
  if (type() != Type::DragDrop) {
    throw std::runtime_error("Wrong data access");
  }
  return drag_drop_;
}

void Event::copy(const Event &other) {
  assign_all(other);
  switch (type()) {
  case Type::DragDrop:
    new (&drag_drop_.filepath) std::string(other.drag_drop().filepath);
    break;
  default:
    break;
  }
}

void Event::move(Event &&other) {
  assign_all(other);
  switch (type()) {
  case Type::DragDrop:
    new (&drag_drop_.filepath) std::string(std::move(other.drag_drop().filepath));
    break;
  default:
    break;
  }
}

void Event::construct_complex_members() {
  switch (type()) {
  case Type::DragDrop:
    new (&drag_drop_.filepath) std::string();
    break;
  default:
    break;
  }
}

void Event::destruct_complex_members() {
  switch (type()) {
  case Type::DragDrop:
    drag_drop_.filepath.~basic_string();
    break;
  default:
    break;
  }
}

void Event::assign_all(const Event &other) {
  switch (type()) {
  case Type::None:
    break;
  case Type::Init:
    init_.width = other.init_.width;
    init_.height = other.init_.height;
    break;
  case Type::Quit:
    break;
  case Type::Resize:
    resize_.width = other.resize_.width;
    resize_.height = other.resize_.height;
    break;
  case Type::Redraw:
    break;
  case Type::MouseButton:
    mouse_button_.action = other.mouse_button_.action;
    mouse_button_.button = other.mouse_button_.button;
    break;
  case Type::MouseMove:
    mouse_move_.x = other.mouse_move_.x;
    mouse_move_.y = other.mouse_move_.y;
    mouse_move_.x_rel = other.mouse_move_.x_rel;
    mouse_move_.y_rel = other.mouse_move_.y_rel;
    mouse_move_.mouse_button_mask = other.mouse_move_.mouse_button_mask;
    break;
  case Type::MouseScroll:
    mouse_scroll_.vertical = other.mouse_scroll_.vertical;
    mouse_scroll_.horizontal = other.mouse_scroll_.horizontal;
    break;
  case Type::Key:
    key_.action = other.key_.action;
    key_.scan_code = other.key_.scan_code;
    break;
  case Type::DragDrop:
    break;
  default:
    throw std::runtime_error("Unhandled event type occurred");
    break;
  }
}
} // namespace gp::misc
