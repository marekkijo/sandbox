#pragma once

#include "streaming_common/constants.hpp"
#include "streaming_common/encoder_fwd.hpp"
#include "streaming_common/video_stream_info.hpp"

#include <gp/misc/event.hpp>

#include <nlohmann/json.hpp>
#include <rtc/rtc.hpp>

#include <functional>
#include <memory>
#include <unordered_set>

namespace streaming {
class Streamer {
public:
  Streamer(const Streamer &) = delete;
  Streamer &operator=(const Streamer &) = delete;
  Streamer(Streamer &&other) noexcept = delete;
  Streamer &operator=(Streamer &&other) noexcept = delete;

  Streamer(const std::string &server_ip, const std::uint16_t server_port, std::shared_ptr<Encoder> encoder);

  void set_event_callback(std::function<void(const gp::misc::Event &event)> event_callback);

private:
  struct Peer {
    std::string id{};
    std::shared_ptr<rtc::PeerConnection> connection{};
    std::shared_ptr<rtc::DataChannel> data_channel{};
  };

  void init_web_socket(std::shared_ptr<rtc::WebSocket> web_socket);

  void on_web_socket_open();
  void on_web_socket_closed();
  void on_web_socket_error(std::string error);
  void on_web_socket_binary_message(rtc::binary message);
  void on_web_socket_string_message(std::string message);

  void on_peer_state_change(rtc::PeerConnection::State state);
  void on_peer_gathering_state_change(rtc::PeerConnection::GatheringState state);

  void on_data_channel_open();
  void on_data_channel_closed();
  void on_data_channel_error(std::string error);
  void on_data_channel_binary_message(rtc::binary message);
  void on_data_channel_string_message(std::string message);

  [[nodiscard]] std::shared_ptr<Peer> create_peer(const std::string &id);
  void send_video_stream_info();
  void video_stream_callback(const std::byte *data, const std::size_t size, const bool eof);
  void parse_event(const nlohmann::json &json_event);

  const std::string id_{};
  const VideoStreamInfo video_stream_info_{};
  bool connection_open_{false};
  rtc::Configuration configuration_{};
  std::shared_ptr<rtc::WebSocket> web_socket_{};
  std::shared_ptr<Peer> peer_{};
  std::function<void(const gp::misc::Event &event)> event_callback_{};
};
} // namespace streaming
