#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
}

#include <string>

namespace streaming {
class Codec {
public:
  Codec(const Codec &)                     = delete;
  Codec &operator=(const Codec &)          = delete;
  Codec(Codec &&other) noexcept            = delete;
  Codec &operator=(Codec &&other) noexcept = delete;

  Codec(int width, int height);
  ~Codec();

private:
  struct StreamingParams {
    char  copy_video;
    char  copy_audio;
    char *muxer_opt_key;
    char *muxer_opt_value;
    char *video_codec;
    char *audio_codec;
    char *codec_priv_key;
    char *codec_priv_value;
  };

  struct StreamingContext {
    AVFormatContext *avfc;
    AVCodec         *video_avc;
    AVCodec         *audio_avc;
    AVStream        *video_avs;
    AVStream        *audio_avs;
    AVCodecContext  *video_avcc;
    AVCodecContext  *audio_avcc;
    int              video_index;
    int              audio_index;
    std::string      filename;
  };

  StreamingParams  params_{};
  StreamingContext context_{};

  AVPacket *output_packet_{nullptr};
};
} // namespace streaming
