#include "codec.hpp"

namespace streaming {
Codec::Codec() : output_packet_{av_packet_alloc()} {
  context_.filename = "/home/marek/works/output.mp4";
  avformat_alloc_output_context2(&context_.avfc, nullptr, nullptr, context_.filename.c_str());
  context_.video_avs  = avformat_new_stream(context_.avfc, nullptr);
  context_.video_avc  = avcodec_find_encoder_by_name("libx264");
  context_.video_avcc = avcodec_alloc_context3(context_.video_avc);
  av_opt_set(context_.video_avcc->priv_data, "preset", "fast", 0);
}

Codec::~Codec() {
  av_packet_unref(output_packet_);
  av_packet_free(&output_packet_);
}
} // namespace streaming
