#pragma once

#include <fstream>
#include <memory>
#include <string>

namespace wolf {
class RawMap;

class RawMapFromAscii {
public:
  RawMapFromAscii(const RawMapFromAscii &) = delete;
  RawMapFromAscii &operator=(const RawMapFromAscii &) = delete;
  RawMapFromAscii(RawMapFromAscii &&) noexcept = default;
  RawMapFromAscii &operator=(RawMapFromAscii &&) noexcept = default;
  ~RawMapFromAscii() = default;

  RawMapFromAscii(const std::string &asciimap_filename);

  [[nodiscard]] std::unique_ptr<RawMap> create_map();

private:
  std::ifstream asciimap_file_{};
};
} // namespace wolf
