#pragma once

#include <gp/ffmpeg/ffmpeg.hpp>

#include <cstdint>
#include <string>

namespace streaming {
struct VideoStreamInfo {
  int width{};
  int height{};
  std::uint16_t fps{};
  AVCodecID codec_id{AV_CODEC_ID_NONE};
  std::string codec_name{};
};
} // namespace streaming
