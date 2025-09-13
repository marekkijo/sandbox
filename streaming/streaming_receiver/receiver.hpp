#pragma once

#include "streaming_common/video_stream_info.hpp"

#include <gp/misc/event.hpp>

#include <nlohmann/json.hpp>
#include <rtc/rtc.hpp>

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace streaming {
class Receiver {
public:
  Receiver(const Receiver &) = delete;
  Receiver &operator=(const Receiver &) = delete;
  Receiver(Receiver &&other) noexcept = delete;
  Receiver &operator=(Receiver &&other) noexcept = delete;

  Receiver(const std::string &server_ip, const std::uint16_t server_port);

  void connect();
  void handle_event(const gp::misc::Event &event);
  void set_video_stream_info_callback(
      std::function<void(const VideoStreamInfo &video_stream_info)> video_stream_info_callback);
  void set_incoming_video_stream_data_callback(
      std::function<void(const std::byte *data, const std::size_t size)> incoming_video_stream_data_callback);

private:
  struct Peer {
    std::string id{};
    std::shared_ptr<rtc::PeerConnection> connection{};
    std::shared_ptr<rtc::DataChannel> data_channel{};
  };

  struct StreamerInfo {
    std::string streamer_id;
    VideoStreamInfo video_stream_info;
  };

  void init_web_socket(std::shared_ptr<rtc::WebSocket> web_socket);

  void on_web_socket_open();
  void on_web_socket_closed();
  void on_web_socket_error(std::string error);
  void on_web_socket_binary_message(rtc::binary message);
  void on_web_socket_string_message(std::string message);

  void on_peer_state_change(rtc::PeerConnection::State state);
  void on_peer_gathering_state_change(rtc::PeerConnection::GatheringState state);
  void on_peer_local_description(rtc::Description description);
  void on_peer_local_candidate(rtc::Candidate candidate);
  void on_peer_data_channel(std::shared_ptr<rtc::DataChannel> data_channel);

  void on_data_channel_open();
  void on_data_channel_closed();
  void on_data_channel_error(std::string error);
  void on_data_channel_binary_message(rtc::binary message);
  void on_data_channel_string_message(std::string message);

  [[nodiscard]] std::shared_ptr<Peer> create_peer(const std::string &id);

  void command_request_video_stream_infos();
  void command_request_video_stream(const std::string &streamer_id);
  void parse_video_stream_infos(const nlohmann::json &json_video_stream_infos);

  const std::string receiver_id_{};
  const std::string id_{};
  bool connection_open_{false};
  rtc::Configuration configuration_{};
  std::string connection_url_;
  std::shared_ptr<rtc::WebSocket> web_socket_{};
  std::shared_ptr<Peer> peer_{};
  std::vector<StreamerInfo> streamer_infos_{};

  std::function<void(const VideoStreamInfo &video_stream_info)> video_stream_info_callback_{};
  std::function<void(const std::byte *data, const std::size_t size)> incoming_video_stream_data_callback_{};
};
} // namespace streaming
