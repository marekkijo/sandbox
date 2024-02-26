#pragma once

#include <gp/ffmpeg/ffmpeg.hpp>

#include <cstdint>
#include <string>

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

constexpr auto ENCODE_BITRATE = BITRATE_kbits_64;

constexpr auto CHANNELS_NUM = int{4};
constexpr auto DECODE_THREAD_COUNT = std::size_t{4u};
constexpr auto ENCODE_THREAD_COUNT = std::size_t{4u};
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
} // namespace streaming
