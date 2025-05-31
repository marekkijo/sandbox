#pragma once

#include "streaming_common/decoder_fwd.hpp"

#include "streaming_common/frame_data.hpp"
#include "streaming_common/video_stream_info.hpp"

#include <gp/ffmpeg/ffmpeg.hpp>

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

namespace streaming {
class Decoder {
public:
  /**
   * Describes the status of the decoding.
   */
  struct Status {
    /**
     * The result of decoding.
     */
    enum class Code {
      OK,     /** frame successfully decoded - @ref frame_num contains a subsequent number of the decoded frame */
      RETRY,  /** packet sent - retry the call to receive frame */
      EOS,    /** reached and processed End Of Stream - it is expected to reset the Decoder with the
                 set_video_stream_info() call */
      NODATA, /** there is no data available to decode another frame */
      ERROR   /** unrecoverable error */
    } code;

    /**
     * Contains a subsequent number of the decoded frame if decodiung was successful, otherwise the value is
     * meaningless.
     */
    int frame_num;
  };

  Decoder() = default;

  ~Decoder();

  Decoder(const Decoder &) = delete;
  Decoder &operator=(const Decoder &) = delete;
  Decoder(Decoder &&other) noexcept = delete;
  Decoder &operator=(Decoder &&other) noexcept = delete;

  void init(const VideoStreamInfo &video_stream_info);

  std::shared_ptr<FrameData> rgb_frame();
  /**
   * Prepares another frame available through @ref rgb_frame().
   */
  [[nodiscard]] Status decode();
  /**
   * Upload stream data to the intermediate buffer.
   *
   * @return
   *      true:   data successfully uploaded
   *      false:  data not uploaded - in EOF state, data must be temporarily stored elsewhere
   */
  bool incoming_data(const std::byte *data, const std::size_t size, const bool async = false);
  void signal_eof();

private:
  [[nodiscard]] bool upload();
  void reduce_buffer(int n);
  void destroy();
  void yuv_to_rgb();
  void fill_async_buffer(const std::byte *data, const std::size_t size);
  void consume_async_buffer();

  static constexpr std::array<std::uint8_t, AV_INPUT_BUFFER_PADDING_SIZE> NULL_PADDING{};

  std::shared_ptr<FrameData> rgb_frame_{};

  const AVCodec *codec_{};
  AVCodecContext *context_{};
  AVCodecParserContext *parser_{};
  AVPacket *packet_{};
  AVFrame *frame_{};

  std::vector<std::uint8_t> buffer_{};

  std::vector<std::uint8_t> async_buffer_{};
  std::mutex async_buffer_mutex_{};

  /**
   * Packet was sent by avcodec_send_packet - expected to receive frames first.
   */
  bool packet_sent_{false};
  /**
   * EOF signaled from the outside - no more data accepted.
   */
  std::atomic_bool signaled_eof_{};

  SwsContext *sws_context_{};
};
} // namespace streaming
