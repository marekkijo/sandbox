#pragma once

#include <cstddef>

namespace streaming {
constexpr auto BITRATE_kbits_1 = 1024 /* kilo */ * 8 /* bits */;
constexpr auto BITRATE_kbits_2 = BITRATE_kbits_1 * 2;
constexpr auto BITRATE_kbits_4 = BITRATE_kbits_1 * 4;
constexpr auto BITRATE_kbits_8 = BITRATE_kbits_1 * 8;
constexpr auto BITRATE_kbits_16 = BITRATE_kbits_1 * 16;
constexpr auto BITRATE_kbits_32 = BITRATE_kbits_1 * 32;
constexpr auto BITRATE_kbits_64 = BITRATE_kbits_1 * 64;
constexpr auto BITRATE_kbits_128 = BITRATE_kbits_1 * 128;
constexpr auto BITRATE_kbits_256 = BITRATE_kbits_1 * 256;
constexpr auto BITRATE_kbits_512 = BITRATE_kbits_1 * 512;
constexpr auto BITRATE_Mbits_1 = BITRATE_kbits_1 * 1024;
constexpr auto BITRATE_Mbits_2 = BITRATE_Mbits_1 * 2;
constexpr auto BITRATE_Mbits_4 = BITRATE_Mbits_1 * 4;
constexpr auto BITRATE_Mbits_8 = BITRATE_Mbits_1 * 8;

constexpr auto ENCODE_BITRATE = BITRATE_kbits_8;

constexpr auto CHANNELS_NUM = int{4};
constexpr auto DECODE_THREAD_COUNT = std::size_t{4u};
constexpr auto ENCODE_THREAD_COUNT = std::size_t{4u};
constexpr auto MAX_MESSAGE_SIZE = 1024u * 1024u; // 1MB
constexpr auto STREAMER_ID = "streamer";
constexpr auto RECEIVER_ID = "receiver";
constexpr auto DATA_CHANNEL_ID = "video-channel";
} // namespace streaming
