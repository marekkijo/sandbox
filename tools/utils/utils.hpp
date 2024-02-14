#pragma once

#include <string>
#include <vector>

namespace tools::utils {
std::vector<std::string> split_by(const std::string &src, const std::string &delimiter);
std::string              generate_random_string(std::size_t len);
std::string              load_txt_file(const std::string &filename);
} // namespace tools::utils
