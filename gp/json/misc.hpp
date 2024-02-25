#pragma once

#include <nlohmann/json.hpp>

#include <gp/misc/event.hpp>

namespace gp::json {
misc::Event to_event(const nlohmann::json &json_event);
nlohmann::json from_event(const misc::Event &event);
} // namespace gp::json
