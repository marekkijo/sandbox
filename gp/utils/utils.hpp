#pragma once

#include <SDL3/SDL.h>
#include <glm/vec2.hpp>

#include <string>
#include <vector>

namespace gp::utils {
std::vector<std::string> split_by(const std::string &src, const std::string &delimiter);
std::string generate_random_string(std::size_t length);
std::string load_txt_file(const std::string &filename);
void set_working_directory();
glm::vec2 orientation_to_dir(const float orientation);
SDL_FRect rect_at(const glm::vec2 &pos, const float size = 1.0f);
} // namespace gp::utils
