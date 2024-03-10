#pragma once

#include "common.hpp"

#include <gp/ffmpeg/ffmpeg.hpp>

#include <array>
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
  Decoder(const Decoder &) = delete;
  Decoder &operator=(const Decoder &) = delete;
  Decoder(Decoder &&other) noexcept = delete;
  Decoder &operator=(Decoder &&other) noexcept = delete;

  ~Decoder();

  std::shared_ptr<std::vector<std::uint8_t>> &rgb_frame();
  void set_video_stream_info_callback(
      std::function<void(const VideoStreamInfo &video_stream_info)> video_stream_info_callback);
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
  [[nodiscard]] bool incoming_data(const std::byte *data, const std::size_t size);
  void signal_eof();
  void set_video_stream_info(const VideoStreamInfo &video_stream_info);

private:
  [[nodiscard]] bool upload();
  void reduce_buffer(int n);
  void destroy();
  void yuv_to_rgb();

  static constexpr std::array<std::uint8_t, AV_INPUT_BUFFER_PADDING_SIZE> NULL_PADDING{};

  std::function<void(const VideoStreamInfo &video_stream_info)> video_stream_info_callback_{};

  std::shared_ptr<std::vector<std::uint8_t>> rgb_frame_{};

  AVPacket *packet_{};
  const AVCodec *codec_{};
  AVCodecParserContext *parser_{};
  AVCodecContext *context_{};
  AVFrame *frame_{};

  std::vector<std::uint8_t> buffer_{};
  /**
   * Packet was sent by avcodec_send_packet - expected to receive frames first.
   */
  bool packet_sent_{false};
  /**
   * EOF signaled from the outside - no more data accepted.
   */
  bool signaled_eof_{false};

  SwsContext *sws_context_{};

  /**
   * incoming_data() is expected to be called in parallel - its critical section must be secured with this mutex.
   */
  std::mutex mutex_{};
};
} // namespace streaming
