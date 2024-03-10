#pragma once

#include <gp/misc/event.hpp>

namespace gp::glfw::internal {
misc::Event::ScanCode to_scan_code(const int glfw_scan_code);
} // namespace gp::glfw::internal
