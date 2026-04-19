#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
}

#include <memory>

namespace gp::ffmpeg {

struct AVCodecContextDeleter {
  void operator()(AVCodecContext *p) const noexcept { avcodec_free_context(&p); }
};
using UniqueAVCodecContext = std::unique_ptr<AVCodecContext, AVCodecContextDeleter>;

struct AVCodecParserContextDeleter {
  void operator()(AVCodecParserContext *p) const noexcept { av_parser_close(p); }
};
using UniqueAVCodecParserContext = std::unique_ptr<AVCodecParserContext, AVCodecParserContextDeleter>;

struct AVPacketDeleter {
  void operator()(AVPacket *p) const noexcept { av_packet_free(&p); }
};
using UniqueAVPacket = std::unique_ptr<AVPacket, AVPacketDeleter>;

struct AVFrameDeleter {
  void operator()(AVFrame *p) const noexcept { av_frame_free(&p); }
};
using UniqueAVFrame = std::unique_ptr<AVFrame, AVFrameDeleter>;

struct SwsContextDeleter {
  void operator()(SwsContext *p) const noexcept { sws_freeContext(p); }
};
using UniqueSwsContext = std::unique_ptr<SwsContext, SwsContextDeleter>;

} // namespace gp::ffmpeg
