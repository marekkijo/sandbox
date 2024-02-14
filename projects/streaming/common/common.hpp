#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
}

#include <cstdint>
#include <string>

namespace streaming {
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
} // namespace streaming
