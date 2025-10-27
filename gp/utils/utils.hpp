#pragma once

#include <string>
#include <vector>

namespace gp::utils {
std::vector<std::string> split_by(const std::string &src, const std::string &delimiter);
std::string generate_random_string(std::size_t length);
std::string load_txt_file(const std::string &filename);
void set_working_directory();
} // namespace gp::utils
