#include "misc.hpp"

#include <stdexcept>

namespace gp::json {
namespace {
misc::Event::Type from_string(const std::string &type_string) {
  if (type_string == "Init") { return misc::Event::Type::Init; }
  if (type_string == "Quit") { return misc::Event::Type::Quit; }
  if (type_string == "Resize") { return misc::Event::Type::Resize; }
  if (type_string == "Redraw") { return misc::Event::Type::Redraw; }
  if (type_string == "MouseButton") { return misc::Event::Type::MouseButton; }
  if (type_string == "MouseMove") { return misc::Event::Type::MouseMove; }
  if (type_string == "MouseScroll") { return misc::Event::Type::MouseScroll; }
  if (type_string == "Key") { return misc::Event::Type::Key; }
  if (type_string == "DragDrop") { return misc::Event::Type::DragDrop; }
  return misc::Event::Type::None;
}
} // namespace

misc::Event to_event(const nlohmann::json &json_event) {
  const auto timestamp = json_event.at("timestamp");
  const auto type = from_string(json_event.at("type"));
  auto event = misc::Event(type, timestamp);

  switch (event.type()) {
  case misc::Event::Type::Init: {
    const auto init = json_event.at("init");
    event.init().width = init.at("width");
    event.init().height = init.at("height");
  } break;
  case misc::Event::Type::Quit: {
    const auto quit = json_event.at("quit");
    event.quit().close_flag = quit.at("close_flag");
  } break;
  case misc::Event::Type::Resize: {
    const auto resize = json_event.at("resize");
    event.resize().width = resize.at("width");
    event.resize().height = resize.at("height");
  } break;
  case misc::Event::Type::Redraw:
    break;
  case misc::Event::Type::MouseButton: {
    const auto mouse_button = json_event.at("mouse_button");
    const auto action = mouse_button.at("action");
    if (action == "Pressed") {
      event.mouse_button().action = misc::Event::Action::Pressed;
    } else if (action == "Released") {
      event.mouse_button().action = misc::Event::Action::Released;
    } else {
      event.mouse_button().action = misc::Event::Action::None;
    }
    const auto button = mouse_button.at("button");
    if (button == "Pressed") {
      event.mouse_button().button = misc::Event::MouseButton::Left;
    } else if (button == "Middle") {
      event.mouse_button().button = misc::Event::MouseButton::Middle;
    } else if (button == "Right") {
      event.mouse_button().button = misc::Event::MouseButton::Right;
    } else {
      event.mouse_button().button = misc::Event::MouseButton::None;
    }
  } break;
  case misc::Event::Type::MouseMove: {
    const auto mouse_move = json_event.at("mouse_move");
    event.mouse_move().x = mouse_move.at("x");
    event.mouse_move().y = mouse_move.at("y");
    event.mouse_move().x_rel = mouse_move.at("x_rel");
    event.mouse_move().y_rel = mouse_move.at("y_rel");
    event.mouse_move().mouse_button_mask = mouse_move.at("mouse_button_mask");
  } break;
  case misc::Event::Type::MouseScroll: {
    const auto mouse_scroll = json_event.at("mouse_scroll");
    event.mouse_scroll().vertical = mouse_scroll.at("vertical");
    event.mouse_scroll().horizontal = mouse_scroll.at("horizontal");
  } break;
  case misc::Event::Type::Key: {
    const auto key = json_event.at("key");
    const auto action = key.at("action");
    if (action == "Pressed") {
      event.key().action = misc::Event::Action::Pressed;
    } else if (action == "Released") {
      event.key().action = misc::Event::Action::Released;
    } else {
      event.key().action = misc::Event::Action::None;
    }
    const auto code = key.at("code");
    if (code == "Down") {
      event.key().code = misc::Event::KeyCode::Down;
    } else if (code == "Left") {
      event.key().code = misc::Event::KeyCode::Left;
    } else if (code == "Right") {
      event.key().code = misc::Event::KeyCode::Right;
    } else if (code == "Up") {
      event.key().code = misc::Event::KeyCode::Up;
    } else if (code == "Escape") {
      event.key().code = misc::Event::KeyCode::Escape;
    } else {
      event.key().code = misc::Event::KeyCode::None;
    }
  } break;
  case misc::Event::Type::DragDrop: {
    const auto drag_drop = json_event.at("drag_drop");
    event.drag_drop().filepath = drag_drop.at("filepath");
  } break;
  default:
    throw std::runtime_error("Unhandled event type occurred");
    break;
  }

  return event;
}

nlohmann::json from_event(const misc::Event &event) {
  auto json_event = nlohmann::json{
      {"timestamp", event.timestamp()},
      {     "type",            "None"},
  };

  switch (event.type()) {
  case misc::Event::Type::None:
    json_event["type"] = "None";
    break;
  case misc::Event::Type::Init:
    json_event["type"] = "Init";

    json_event["init"] = nlohmann::json{
        { "width",  event.init().width},
        {"height", event.init().height}
    };
    break;
  case misc::Event::Type::Quit:
    json_event["type"] = "Quit";

    json_event["quit"] = nlohmann::json{
        {"close_flag", event.quit().close_flag}
    };
    break;
  case misc::Event::Type::Resize:
    json_event["type"] = "Resize";

    json_event["resize"] = nlohmann::json{
        { "width",  event.resize().width},
        {"height", event.resize().height}
    };
    break;
  case misc::Event::Type::Redraw:
    json_event["type"] = "Redraw";
    break;
  case misc::Event::Type::MouseButton: {
    json_event["type"] = "MouseButton";

    auto action = std::string("None");
    switch (event.mouse_button().action) {
    case misc::Event::Action::Pressed:
      action = "Pressed";
      break;
    case misc::Event::Action::Released:
      action = "Released";
      break;
    default:
      break;
    }

    auto button = std::string("None");
    switch (event.mouse_button().button) {
    case misc::Event::MouseButton::Left:
      button = "Left";
      break;
    case misc::Event::MouseButton::Middle:
      button = "Middle";
      break;
    case misc::Event::MouseButton::Right:
      button = "Right";
      break;
    default:
      break;
    }

    json_event["mouse_button"] = nlohmann::json{
        {"action", action},
        {"button", button}
    };
  } break;
  case misc::Event::Type::MouseMove:
    json_event["type"] = "MouseMove";

    json_event["mouse_move"] = nlohmann::json{
        {                "x",                 event.mouse_move().x},
        {                "y",                 event.mouse_move().y},
        {            "x_rel",             event.mouse_move().x_rel},
        {            "y_rel",             event.mouse_move().y_rel},
        {"mouse_button_mask", event.mouse_move().mouse_button_mask}
    };
    break;
  case misc::Event::Type::MouseScroll:
    json_event["type"] = "MouseScroll";

    json_event["mouse_scroll"] = nlohmann::json{
        {  "vertical",   event.mouse_scroll().vertical},
        {"horizontal", event.mouse_scroll().horizontal}
    };
    break;
  case misc::Event::Type::Key: {
    json_event["type"] = "Key";

    auto action = std::string("None");
    switch (event.key().action) {
    case misc::Event::Action::Pressed:
      action = "Pressed";
      break;
    case misc::Event::Action::Released:
      action = "Released";
      break;
    default:
      break;
    }

    auto code = std::string("None");
    switch (event.key().code) {
    case misc::Event::KeyCode::Down:
      code = "Down";
      break;
    case misc::Event::KeyCode::Left:
      code = "Left";
      break;
    case misc::Event::KeyCode::Right:
      code = "Right";
      break;
    case misc::Event::KeyCode::Up:
      code = "Up";
      break;
    case misc::Event::KeyCode::Escape:
      code = "Escape";
      break;
    default:
      break;
    }

    json_event["key"] = nlohmann::json{
        {"action", action},
        {  "code",   code}
    };
  } break;
  case misc::Event::Type::DragDrop:
    json_event["type"] = "DragDrop";

    json_event["drag_drop"] = nlohmann::json{
        {"filepath", event.drag_drop().filepath}
    };
    break;
  default:
    throw std::runtime_error("Unhandled event type occurred");
    break;
  }

  return json_event;
}
} // namespace gp::json
