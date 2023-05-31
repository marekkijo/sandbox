#pragma once

#include <fstream>
#include <memory>
#include <string>

namespace wolf {
class RawMap;

class RawMapFromAscii {
public:
  RawMapFromAscii(const RawMapFromAscii &)                = default;
  RawMapFromAscii &operator=(const RawMapFromAscii &)     = default;
  RawMapFromAscii(RawMapFromAscii &&) noexcept            = default;
  RawMapFromAscii &operator=(RawMapFromAscii &&) noexcept = default;
  ~RawMapFromAscii()                                      = default;

  RawMapFromAscii(const std::string &asciimap_filename);

  [[nodiscard]] std::unique_ptr<RawMap> create_map();

private:
  std::ifstream asciimap_file_{};
};
} // namespace wolf
