#pragma once

#include <gp/ffmpeg/ffmpeg.hpp>

#include <string>

namespace gp::ffmpeg {
AVCodecID codec_name_to_id(const std::string &codec_name);
} // namespace gp::ffmpeg
