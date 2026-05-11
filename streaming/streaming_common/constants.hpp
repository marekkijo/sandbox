#pragma once

#include <cstddef>
#include <cstdint>

namespace streaming {
constexpr auto BITRATE_kbits_1 = 1024; // 1 kbit/s in bits/s (binary)
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

constexpr auto ENCODE_BITRATE = BITRATE_Mbits_4;

constexpr auto CHANNELS_NUM = int{4};
constexpr auto MAX_MESSAGE_SIZE = 1024u * 1024u; // 1MB
constexpr auto STREAMER_ID = "streamer";
constexpr auto RECEIVER_ID = "receiver";
constexpr auto DATA_CHANNEL_ID = "video-channel";

// Feedback ACK: receiver sends one ACK message every ACK_INTERVAL received DataChannel packets.
constexpr auto ACK_INTERVAL = std::size_t{10};

// Lag thresholds (in DataChannel packets) for encoder throttling on the streamer side.
// Each encoded frame produces one packet, so packet lag approximates frame lag in normal operation.
// Above LAG_THROTTLE_HEAVY: encode every 4th frame.
// Above LAG_THROTTLE_LIGHT: encode every 2nd frame.
constexpr auto LAG_THROTTLE_LIGHT = std::uint64_t{10};
constexpr auto LAG_THROTTLE_HEAVY = std::uint64_t{30};
} // namespace streaming
