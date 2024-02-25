#include "receiver.hpp"

#include <gp/json/misc.hpp>
#include <gp/utils/utils.hpp>

#include <SDL2/SDL_events.h>

namespace streaming {
Receiver::Receiver(const std::string &server_ip,
                   const std::uint16_t server_port,
                   std::shared_ptr<Decoder> &decoder,
                   std::shared_ptr<Player> &player)
    : receiver_id_{gp::utils::generate_random_string(16u)}
    , id_{std::string{RECEIVER_ID} + ":" + receiver_id_}
    , decoder_{decoder} {
  configuration_.maxMessageSize = MAX_MESSAGE_SIZE;

  web_socket_ = std::make_shared<rtc::WebSocket>();
  init_web_socket(web_socket_);
  const auto url = std::string{"ws://"} + server_ip + ":" + std::to_string(server_port) + "/" + id_;
  printf("Connection url: %s\n", url.c_str());
  web_socket_->open(url);

  auto event_function = std::function<void(const gp::misc::Event &event)>{
      std::bind(&Receiver::event_callback, this, std::placeholders::_1)};
  player->set_event_callback(event_function);
}

void Receiver::init_web_socket(std::shared_ptr<rtc::WebSocket> web_socket) {
  auto on_web_socket_open_function = std::function<void()>{std::bind(&Receiver::on_web_socket_open, this)};
  auto on_web_socket_closed_function = std::function<void()>{std::bind(&Receiver::on_web_socket_closed, this)};
  auto on_web_socket_error_function =
      std::function<void(std::string error)>{std::bind(&Receiver::on_web_socket_error, this, std::placeholders::_1)};
  auto on_web_socket_binary_message_function = std::function<void(rtc::binary message)>{
      std::bind(&Receiver::on_web_socket_binary_message, this, std::placeholders::_1)};
  auto on_web_socket_string_message_function = std::function<void(std::string message)>{
      std::bind(&Receiver::on_web_socket_string_message, this, std::placeholders::_1)};

  web_socket->onOpen(on_web_socket_open_function);
  web_socket->onClosed(on_web_socket_closed_function);
  web_socket->onError(on_web_socket_error_function);
  web_socket->onMessage(on_web_socket_binary_message_function, on_web_socket_string_message_function);
}

void Receiver::on_web_socket_open() {
  printf("Connection established\n");
  connection_open_ = true;
  command_request_video_stream_infos();
}

void Receiver::on_web_socket_closed() {
  printf("Connection closed\n");
  connection_open_ = false;
}

void Receiver::on_web_socket_error(std::string error) { printf("Connection error: %s\n", error.c_str()); }

void Receiver::on_web_socket_binary_message(rtc::binary /* message */) {
  printf("Received websocket binary message\n");
}

void Receiver::on_web_socket_string_message(std::string message) {
  auto json = nlohmann::json::parse(message);

  if (json.contains("id") && json.contains("type") && json.contains("sdp")) {
    std::string id = json.at("id");
    std::string type = json.at("type");
    std::string sdp = json.at("sdp");

    if (type == "offer") {
      peer_ = create_peer(id);
      auto connection = peer_->connection;
      auto description = rtc::Description{sdp, type};
      peer_->connection->setRemoteDescription(description);
      return;
    }
  } else if (json.contains("video_stream_infos")) {
    parse_video_stream_infos(json.at("video_stream_infos"));
    return;
  }

  printf("Unknown message: %s\n", message.c_str());
}

void Receiver::on_peer_state_change(rtc::PeerConnection::State state) {
  switch (state) {
  case rtc::PeerConnection::State::Connecting:
    printf("Peer state: Connecting\n");
    break;
  case rtc::PeerConnection::State::Connected:
    printf("Peer state: Connected\n");
    break;
  case rtc::PeerConnection::State::Disconnected:
    printf("Peer state: Disconnected\n");
    peer_ = nullptr;
    break;
  case rtc::PeerConnection::State::Failed:
    printf("Peer state: Failed\n");
    peer_ = nullptr;
    break;
  case rtc::PeerConnection::State::Closed:
    printf("Peer state: Closed\n");
    peer_ = nullptr;
    break;

  default:
    break;
  }
}

void Receiver::on_peer_gathering_state_change(rtc::PeerConnection::GatheringState state) {
  if (state == rtc::PeerConnection::GatheringState::Complete) {
    auto description = peer_->connection->localDescription();
    const auto json = nlohmann::json{
        {  "id",                        peer_->id},
        {"type",        description->typeString()},
        { "sdp", std::string(description.value())}
    };
    web_socket_->send(json.dump());
  }
}

void Receiver::on_peer_local_description(rtc::Description description) {
  const auto json = nlohmann::json{
      {         "id",                peer_->id},
      {       "type", description.typeString()},
      {"description", std::string(description)}
  };
  web_socket_->send(json.dump());
}

void Receiver::on_peer_local_candidate(rtc::Candidate candidate) {
  const auto json = nlohmann::json{
      {       "id",              peer_->id},
      {     "type",            "candidate"},
      {"candidate", std::string(candidate)},
      {      "mid",        candidate.mid()}
  };
  web_socket_->send(json.dump());
}

void Receiver::on_peer_data_channel(std::shared_ptr<rtc::DataChannel> data_channel) {
  peer_->data_channel = data_channel;

  auto on_data_channel_open_function = std::function<void()>{std::bind(&Receiver::on_data_channel_open, this)};
  auto on_data_channel_closed_function = std::function<void()>{std::bind(&Receiver::on_data_channel_closed, this)};
  auto on_data_channel_error_function =
      std::function<void(std::string error)>{std::bind(&Receiver::on_data_channel_error, this, std::placeholders::_1)};
  auto on_data_channel_binary_message_function = std::function<void(rtc::binary message)>{
      std::bind(&Receiver::on_data_channel_binary_message, this, std::placeholders::_1)};
  auto on_data_channel_string_message_function = std::function<void(std::string message)>{
      std::bind(&Receiver::on_data_channel_string_message, this, std::placeholders::_1)};

  peer_->data_channel->onOpen(on_data_channel_open_function);
  peer_->data_channel->onClosed(on_data_channel_closed_function);
  peer_->data_channel->onError(on_data_channel_error_function);
  peer_->data_channel->onMessage(on_data_channel_binary_message_function, on_data_channel_string_message_function);
}

void Receiver::on_data_channel_open() { printf("Data channel opened\n"); }

void Receiver::on_data_channel_closed() { printf("Data channel closed\n"); }

void Receiver::on_data_channel_error(std::string error) { printf("Data channel error: %s\n", error.c_str()); }

void Receiver::on_data_channel_binary_message(rtc::binary message) {
  if (!decoder_->incoming_data(message.data(), message.size())) { printf("Data upload failed\n"); }
}

void Receiver::on_data_channel_string_message(std::string /* message */) {
  printf("Received data channel string message\n");
}

std::shared_ptr<Receiver::Peer> Receiver::create_peer(const std::string &id) {
  auto peer = std::make_shared<Peer>();
  peer->id = id;
  peer->connection = std::make_shared<rtc::PeerConnection>(configuration_);

  auto on_peer_state_change_function = std::function<void(rtc::PeerConnection::State state)>{
      std::bind(&Receiver::on_peer_state_change, this, std::placeholders::_1)};
  auto on_peer_gathering_state_change_function = std::function<void(rtc::PeerConnection::GatheringState state)>{
      std::bind(&Receiver::on_peer_gathering_state_change, this, std::placeholders::_1)};
  auto on_peer_local_description_function = std::function<void(rtc::Description description)>{
      std::bind(&Receiver::on_peer_local_description, this, std::placeholders::_1)};
  auto on_peer_local_candidate_function = std::function<void(rtc::Candidate candidate)>{
      std::bind(&Receiver::on_peer_local_candidate, this, std::placeholders::_1)};
  auto on_peer_data_channel_function = std::function<void(std::shared_ptr<rtc::DataChannel> data_channel)>{
      std::bind(&Receiver::on_peer_data_channel, this, std::placeholders::_1)};
  peer->connection->onStateChange(on_peer_state_change_function);
  peer->connection->onGatheringStateChange(on_peer_gathering_state_change_function);
  peer->connection->onLocalDescription(on_peer_local_description_function);
  peer->connection->onLocalCandidate(on_peer_local_candidate_function);
  peer->connection->onDataChannel(on_peer_data_channel_function);

  return peer;
}

void Receiver::command_request_video_stream_infos() {
  const auto command_json = nlohmann::json{
      {"type", "request_video_stream_infos"}
  };
  const auto json = nlohmann::json{
      {"command", command_json}
  };
  web_socket_->send(json.dump());
}

void Receiver::command_request_video_stream(const std::string &streamer_id) {
  const auto command_json = nlohmann::json{
      {    "type", "request_video_stream"},
      {"streamer",            streamer_id},
      {"receiver",           receiver_id_}
  };
  const auto json = nlohmann::json{
      {"command", command_json}
  };
  web_socket_->send(json.dump());
}

void Receiver::parse_video_stream_infos(const nlohmann::json &json_video_stream_infos) {
  streamer_infos_.clear();
  for (auto it = json_video_stream_infos.begin(); it != json_video_stream_infos.end(); ++it) {
    streamer_infos_.emplace_back();
    streamer_infos_.back().streamer_id = it->at("streamer_id");
    streamer_infos_.back().video_stream_info.width = it->at("width");
    streamer_infos_.back().video_stream_info.height = it->at("height");
    streamer_infos_.back().video_stream_info.fps = it->at("fps");
    streamer_infos_.back().video_stream_info.codec_id = it->at("codec_id");
    streamer_infos_.back().video_stream_info.codec_name = it->at("codec_name");

    decoder_->set_video_stream_info(streamer_infos_.back().video_stream_info);
    command_request_video_stream(streamer_infos_.back().streamer_id);
  }
}

void Receiver::event_callback(const gp::misc::Event &event) {
  if (peer_ && peer_->data_channel && peer_->data_channel->isOpen()) {
    const auto json_event = gp::json::from_event(event);
    peer_->data_channel->send(json_event.dump());
  }
}
} // namespace streaming
