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
// Custom deleters for FFmpeg RAII wrappers
struct AVCodecContextDeleter {
  void operator()(AVCodecContext *ctx) const noexcept {
    if (ctx) {
      avcodec_free_context(&ctx);
    }
  }
};

struct AVCodecParserContextDeleter {
  void operator()(AVCodecParserContext *parser) const noexcept {
    if (parser) {
      av_parser_close(parser);
    }
  }
};

struct AVPacketDeleter {
  void operator()(AVPacket *packet) const noexcept {
    if (packet) {
      av_packet_free(&packet);
    }
  }
};

struct AVFrameDeleter {
  void operator()(AVFrame *frame) const noexcept {
    if (frame) {
      av_frame_free(&frame);
    }
  }
};

struct SwsContextDeleter {
  void operator()(SwsContext *ctx) const noexcept {
    if (ctx) {
      sws_freeContext(ctx);
    }
  }
};

// RAII unique_ptr aliases
using UniqueAVCodecContext = std::unique_ptr<AVCodecContext, AVCodecContextDeleter>;
using UniqueAVCodecParserContext = std::unique_ptr<AVCodecParserContext, AVCodecParserContextDeleter>;
using UniqueAVPacket = std::unique_ptr<AVPacket, AVPacketDeleter>;
using UniqueAVFrame = std::unique_ptr<AVFrame, AVFrameDeleter>;
using UniqueSwsContext = std::unique_ptr<SwsContext, SwsContextDeleter>;
} // namespace gp::ffmpeg
