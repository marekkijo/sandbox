#include "misc.hpp"

namespace gp::json {
misc::Event to_event(const nlohmann::json &json_event) {
  auto event = misc::Event{};
  event.timestamp = json_event.at("timestamp");

  const auto type = json_event.at("type");
  if (type == "MouseButton") {
    event.type = misc::Event::Type::MouseButton;
    const auto mouse_button = json_event.at("mouse_button");

    const auto action = mouse_button.at("action");
    if (action == "Pressed") {
      event.mouse_button.action = misc::Event::Action::Pressed;
    } else if (action == "Released") {
      event.mouse_button.action = misc::Event::Action::Released;
    } else {
      event.mouse_button.action = misc::Event::Action::None;
    }

    const auto button = mouse_button.at("button");
    if (button == "Pressed") {
      event.mouse_button.button = misc::Event::MouseButton::Left;
    } else if (button == "Middle") {
      event.mouse_button.button = misc::Event::MouseButton::Middle;
    } else if (button == "Right") {
      event.mouse_button.button = misc::Event::MouseButton::Right;
    } else {
      event.mouse_button.button = misc::Event::MouseButton::None;
    }
  } else if (type == "MouseMove") {
    event.type = misc::Event::Type::MouseMove;
    const auto mouse_move = json_event.at("mouse_move");

    event.mouse_move.x = mouse_move.at("x");
    event.mouse_move.y = mouse_move.at("y");
    event.mouse_move.x_rel = mouse_move.at("x_rel");
    event.mouse_move.y_rel = mouse_move.at("y_rel");
    event.mouse_move.mouse_button_mask = mouse_move.at("mouse_button_mask");
  } else if (type == "MouseScroll") {
    event.type = misc::Event::Type::MouseScroll;
    const auto mouse_scroll = json_event.at("mouse_scroll");

    event.mouse_scroll.vertical = mouse_scroll.at("vertical");
    event.mouse_scroll.horizontal = mouse_scroll.at("horizontal");
  } else if (type == "Key") {
    event.type = misc::Event::Type::Key;
    const auto key = json_event.at("key");

    const auto action = key.at("action");
    if (action == "Pressed") {
      event.key.action = misc::Event::Action::Pressed;
    } else if (action == "Released") {
      event.key.action = misc::Event::Action::Released;
    } else {
      event.key.action = misc::Event::Action::None;
    }

    const auto code = key.at("code");
    if (code == "Down") {
      event.key.code = misc::Event::KeyCode::Down;
    } else if (code == "Left") {
      event.key.code = misc::Event::KeyCode::Left;
    } else if (code == "Right") {
      event.key.code = misc::Event::KeyCode::Right;
    } else if (code == "Up") {
      event.key.code = misc::Event::KeyCode::Up;
    } else if (code == "Escape") {
      event.key.code = misc::Event::KeyCode::Escape;
    } else {
      event.key.code = misc::Event::KeyCode::None;
    }
  } else {
    event.type = misc::Event::Type::None;
  }

  return event;
}

nlohmann::json from_event(const misc::Event &event) {
  auto json_event = nlohmann::json{
      {"timestamp", event.timestamp},
      {     "type",          "None"},
  };

  switch (event.type) {
  case misc::Event::Type::MouseButton: {
    json_event["type"] = "MouseButton";

    auto action = std::string("None");
    switch (event.mouse_button.action) {
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
    switch (event.mouse_button.button) {
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
        {                "x",                 event.mouse_move.x},
        {                "y",                 event.mouse_move.y},
        {            "x_rel",             event.mouse_move.x_rel},
        {            "y_rel",             event.mouse_move.y_rel},
        {"mouse_button_mask", event.mouse_move.mouse_button_mask}
    };
    break;
  case misc::Event::Type::MouseScroll:
    json_event["type"] = "MouseScroll";

    json_event["mouse_scroll"] = nlohmann::json{
        {  "vertical",   event.mouse_scroll.vertical},
        {"horizontal", event.mouse_scroll.horizontal}
    };
    break;
  case misc::Event::Type::Key: {
    json_event["type"] = "Key";

    auto action = std::string("None");
    switch (event.key.action) {
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
    switch (event.key.code) {
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
  default:
    break;
  }

  return json_event;
}
} // namespace gp::json
