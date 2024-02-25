#pragma once

#include <string>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
}

namespace gp::ffmpeg {
AVCodecID codec_name_to_id(const std::string &codec_name);
} // namespace gp::ffmpeg
