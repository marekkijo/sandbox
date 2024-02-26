#pragma once

#include <gp/misc/event.hpp>

#include <nlohmann/json.hpp>

namespace gp::json {
misc::Event to_event(const nlohmann::json &json_event);
nlohmann::json from_event(const misc::Event &event);
} // namespace gp::json
