#pragma once

#include <cstdint>
#include <string>

#include "ffmpeg.hpp"

namespace streaming {
constexpr auto BITRATE_kbits_8 = 1024 /* kilo */ * 8 /* bits */ * 8;
constexpr auto BITRATE_kbits_16 = BITRATE_kbits_8 * 2;
constexpr auto BITRATE_kbits_32 = BITRATE_kbits_8 * 4;
constexpr auto BITRATE_kbits_64 = BITRATE_kbits_8 * 8;
constexpr auto BITRATE_kbits_128 = BITRATE_kbits_8 * 16;
constexpr auto BITRATE_kbits_256 = BITRATE_kbits_8 * 32;
constexpr auto BITRATE_kbits_512 = BITRATE_kbits_8 * 64;
constexpr auto BITRATE_Mbits_1 = BITRATE_kbits_8 * 128;
constexpr auto BITRATE_Mbits_2 = BITRATE_kbits_8 * 256;
constexpr auto BITRATE_Mbits_4 = BITRATE_kbits_8 * 512;
constexpr auto BITRATE_Mbits_8 = BITRATE_kbits_8 * 1024;

constexpr auto CHANNELS_NUM = std::size_t{4u};
constexpr auto MAX_MESSAGE_SIZE = 1024u * 1024u; // 1MB
constexpr auto STREAMER_ID = "streamer";
constexpr auto RECEIVER_ID = "receiver";
constexpr auto DATA_CHANNEL_ID = "video-channel";

struct VideoStreamInfo {
  int width{};
  int height{};
  std::uint16_t fps{};
  AVCodecID codec_id{AV_CODEC_ID_NONE};
  std::string codec_name{};
};

struct StreamPackageHeader {
  std::size_t frame_num{};
  bool eof{};
};

constexpr auto STREAM_PACKAGE_HEADER_SIZE = sizeof(StreamPackageHeader);

struct UserInput {
  std::uint32_t type{};
  std::uint32_t timestamp{};
  std::uint32_t state{};
  std::int32_t x{};
  std::int32_t y{};
  std::int32_t x_relative{};
  std::int32_t y_relative{};
  std::uint8_t button{};
  std::uint8_t clicks{};
  float x_float{};
  float y_float{};
  std::uint8_t repeat{};
  std::uint16_t keysym_scancode{};
  std::int32_t keysym_sym{};
  std::uint16_t keysym_mod{};
};

/*
struct UserInput {
  enum class Type : std::uint32_t {
    None,
    MouseMotion,
    MouseButtonDown,
    MouseButtonUp,
    MouseWheel,
    KeyDown,
    KeyUp
  };

  using MaskType = std::uint32_t;

  enum Mask : MaskType {
    None = 0x0,
    LeftMouseButton = 0x1,
    RightMouseButton = 0x2,
    MiddleMouseButton = 0x4
  };

  Type type{Type::None};
  std::uint32_t timestamp{};
  std::uint32_t state{};
  double x{};
  double y{};
  double x_relative{};
  double y_relative{};
  std::uint8_t button{};
  std::uint8_t repeat{};
  std::int32_t keycode{};
};*/
} // namespace streaming
