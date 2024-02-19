#pragma once

#include "player.hpp"
#include "streaming_common/decoder.hpp"

#include <gp/common/user_input.hpp>

#include <nlohmann/json.hpp>
#include <rtc/rtc.hpp>

#include <memory>
#include <unordered_set>
#include <vector>

namespace streaming {
class Receiver {
public:
  Receiver(const Receiver &) = delete;
  Receiver &operator=(const Receiver &) = delete;
  Receiver(Receiver &&other) noexcept = delete;
  Receiver &operator=(Receiver &&other) noexcept = delete;

  Receiver(const std::string &server_ip,
           const std::uint16_t server_port,
           std::shared_ptr<Decoder> &decoder,
           std::shared_ptr<Player> &player);

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

  void user_input_callback(const gp::common::UserInput &user_input);

  const std::string receiver_id_{};
  const std::string id_{};
  bool connection_open_{false};
  rtc::Configuration configuration_{};
  std::shared_ptr<rtc::WebSocket> web_socket_{};
  std::shared_ptr<Peer> peer_{};
  std::shared_ptr<Decoder> decoder_{};
  std::vector<StreamerInfo> streamer_infos_{};
};
} // namespace streaming
