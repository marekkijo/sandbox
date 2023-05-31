#pragma once

#include <fstream>
#include <memory>
#include <string>

namespace wolf {
class RawMap;

class RawMapFromWolf {
public:
  RawMapFromWolf(const RawMapFromWolf &)                = default;
  RawMapFromWolf &operator=(const RawMapFromWolf &)     = default;
  RawMapFromWolf(RawMapFromWolf &&) noexcept            = default;
  RawMapFromWolf &operator=(RawMapFromWolf &&) noexcept = default;
  ~RawMapFromWolf()                                     = default;

  RawMapFromWolf(const std::string &maphead_filename, const std::string &gamemaps_filename);

  [[nodiscard]] std::unique_ptr<RawMap> create_map(const std::size_t map_index);

private:
  std::ifstream maphead_file_{};
  std::ifstream gamemaps_file_{};
};
} // namespace wolf
