#include "utils.hpp"

#include <fstream>
#include <random>
#include <sstream>

namespace gp::utils {
std::vector<std::string> split_by(const std::string &src, const std::string &delimiter) {
  auto tokens = std::vector<std::string>{};
  auto pos = std::string::npos;
  auto ppos = std::size_t{0u};
  while ((pos = src.find(delimiter, ppos)) != std::string::npos) {
    tokens.emplace_back(src.substr(ppos, pos));
    ppos += tokens.back().length() + 1;
  }
  tokens.emplace_back(src.substr(ppos, src.length() - 1));
  return tokens;
}

std::string generate_random_string(const std::size_t length) {
  constexpr auto chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  std::string result{};
  result.reserve(length);

  std::random_device random_device;
  std::mt19937 generator(random_device());
  std::uniform_int_distribution<> distribution(0, 61);

  for (std::size_t i = 0; i < length; i++) {
    result.push_back(chars[distribution(generator)]);
  }

  return result;
}

std::string load_txt_file(const std::string &filename) {
  auto txt_file = std::ifstream{filename};
  auto buffer = std::stringstream{};
  buffer << txt_file.rdbuf();
  return buffer.str();
}
} // namespace gp::utils
