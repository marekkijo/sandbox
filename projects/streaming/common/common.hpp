#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
}

#include <cstdint>
#include <string>

namespace streaming {
constexpr auto CHANNELS_NUM    = std::size_t{4u};
constexpr auto STREAMER_ID     = "streamer";
constexpr auto RECEIVER_ID     = "receiver";
constexpr auto DATA_CHANNEL_ID = "video-channel";

struct VideoStreamInfo {
  int           width{};
  int           height{};
  std::uint16_t fps{};
  AVCodecID     codec_id{AV_CODEC_ID_NONE};
  std::string   codec_name{};
};
} // namespace streaming
