#pragma once

#include <cstddef>

namespace streaming {
struct StreamPackageHeader {
  std::size_t frame_num{};
  bool eof{};
};

constexpr auto STREAM_PACKAGE_HEADER_SIZE = sizeof(StreamPackageHeader);
} // namespace streaming
