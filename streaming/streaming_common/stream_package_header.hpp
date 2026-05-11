#pragma once

#include <array>
#include <cstdint>

namespace streaming {

// Wire format (8 bytes, little-endian):
//   [0..7] frame_num  — uint64, little-endian
//   [8]    flags      — bit 0 = eof
constexpr auto STREAM_PACKAGE_HEADER_SIZE = std::size_t{9};

struct StreamPackageHeader {
  std::uint64_t frame_num{};
  bool eof{};

  [[nodiscard]] std::array<std::uint8_t, STREAM_PACKAGE_HEADER_SIZE> serialize() const noexcept {
    std::array<std::uint8_t, STREAM_PACKAGE_HEADER_SIZE> buf{};
    buf[0] = static_cast<std::uint8_t>(frame_num >> 0);
    buf[1] = static_cast<std::uint8_t>(frame_num >> 8);
    buf[2] = static_cast<std::uint8_t>(frame_num >> 16);
    buf[3] = static_cast<std::uint8_t>(frame_num >> 24);
    buf[4] = static_cast<std::uint8_t>(frame_num >> 32);
    buf[5] = static_cast<std::uint8_t>(frame_num >> 40);
    buf[6] = static_cast<std::uint8_t>(frame_num >> 48);
    buf[7] = static_cast<std::uint8_t>(frame_num >> 56);
    buf[8] = eof ? std::uint8_t{1} : std::uint8_t{0};
    return buf;
  }

  static StreamPackageHeader deserialize(const std::uint8_t *buf) noexcept {
    StreamPackageHeader h{};
    h.frame_num = (static_cast<std::uint64_t>(buf[0]) << 0) | (static_cast<std::uint64_t>(buf[1]) << 8) |
                  (static_cast<std::uint64_t>(buf[2]) << 16) | (static_cast<std::uint64_t>(buf[3]) << 24) |
                  (static_cast<std::uint64_t>(buf[4]) << 32) | (static_cast<std::uint64_t>(buf[5]) << 40) |
                  (static_cast<std::uint64_t>(buf[6]) << 48) | (static_cast<std::uint64_t>(buf[7]) << 56);
    h.eof = (buf[8] & 0x01u) != 0u;
    return h;
  }
};

} // namespace streaming
