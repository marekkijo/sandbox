#include "streamer.hpp"

#include "tools/utils/utils.hpp"

namespace streaming {
Streamer::Streamer(const std::string &server_ip, const std::uint16_t server_port, std::shared_ptr<Encoder> &encoder)
    : id_{std::string{STREAMER_ID} + ":" + tools::utils::generate_random_string(16u)}
    , video_stream_info_{encoder->get_video_stream_info()} {
  std::size_t max_message_size = 1024 * 1024; // 1MB

  configuration_.iceServers.emplace_back("stun.l.google.com", 19302);
  configuration_.disableAutoNegotiation = true;
  configuration_.maxMessageSize = max_message_size;

  web_socket_ = std::make_shared<rtc::WebSocket>();
  init_web_socket(web_socket_);
  const auto url = std::string{"ws://"} + server_ip + ":" + std::to_string(server_port) + "/" + id_;
  printf("Connection url: %s\n", url.c_str());
  web_socket_->open(url);

  auto video_stream_function = std::function<void(const std::byte *data, const std::size_t size)>{
      std::bind(&Streamer::video_stream_callback, this, std::placeholders::_1, std::placeholders::_2)};
  encoder->set_video_stream_callback(video_stream_function);
}

void Streamer::init_web_socket(std::shared_ptr<rtc::WebSocket> web_socket) {
  auto on_web_socket_open_function = std::function<void()>{std::bind(&Streamer::on_web_socket_open, this)};
  auto on_web_socket_closed_function = std::function<void()>{std::bind(&Streamer::on_web_socket_closed, this)};
  auto on_web_socket_error_function =
      std::function<void(std::string error)>{std::bind(&Streamer::on_web_socket_error, this, std::placeholders::_1)};
  auto on_web_socket_binary_message_function = std::function<void(rtc::binary message)>{
      std::bind(&Streamer::on_web_socket_binary_message, this, std::placeholders::_1)};
  auto on_web_socket_string_message_function = std::function<void(std::string message)>{
      std::bind(&Streamer::on_web_socket_string_message, this, std::placeholders::_1)};

  web_socket->onOpen(on_web_socket_open_function);
  web_socket->onClosed(on_web_socket_closed_function);
  web_socket->onError(on_web_socket_error_function);
  web_socket->onMessage(on_web_socket_binary_message_function, on_web_socket_string_message_function);
}

void Streamer::on_web_socket_open() {
  printf("Connection established\n");
  connection_open_ = true;
  send_video_stream_info();
}

void Streamer::on_web_socket_closed() {
  printf("Connection closed\n");
  connection_open_ = false;
}

void Streamer::on_web_socket_error(std::string error) { printf("Connection error: %s\n", error.c_str()); }

void Streamer::on_web_socket_binary_message(rtc::binary /* message */) {
  printf("Received websocket binary message\n");
}

void Streamer::on_web_socket_string_message(std::string message) {
  auto json = nlohmann::json::parse(message);

  if (json.contains("id") && json.contains("type") && json.contains("sdp")) {
    std::string id = json.at("id");
    std::string type = json.at("type");
    std::string sdp = json.at("sdp");

    if (type == "answer") {
      if (peer_ && id == peer_->id) {
        auto connection = peer_->connection;
        auto description = rtc::Description{sdp, type};
        connection->setRemoteDescription(description);
      }
      return;
    }
  } else if (json.contains("request_video_stream")) {
    if (peer_) { return; }

    std::string streamer = json.at("request_video_stream").at("streamer");
    std::string receiver = json.at("request_video_stream").at("receiver");

    peer_ = create_peer(receiver);
    return;
  }

  printf("Unknown message: %s\n", message.c_str());
}

void Streamer::on_data_channel_open() { printf("Data channel opened\n"); }

void Streamer::on_data_channel_closed() { printf("Data channel closed\n"); }

void Streamer::on_data_channel_error(std::string error) { printf("Data channel error: %s\n", error.c_str()); }

void Streamer::on_data_channel_binary_message(rtc::binary /* message */) {
  printf("Received data channel binary message\n");
}

void Streamer::on_data_channel_string_message(std::string /* message */) {
  printf("Received data channel string message\n");
}

void Streamer::on_peer_state_change(rtc::PeerConnection::State state) {
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

void Streamer::on_peer_gathering_state_change(rtc::PeerConnection::GatheringState state) {
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

std::shared_ptr<Streamer::Peer> Streamer::create_peer(const std::string &id) {
  auto peer = std::make_shared<Peer>();
  peer->id = id;
  peer->connection = std::make_shared<rtc::PeerConnection>(configuration_);

  auto on_peer_state_change_function = std::function<void(rtc::PeerConnection::State state)>{
      std::bind(&Streamer::on_peer_state_change, this, std::placeholders::_1)};
  auto on_peer_gathering_state_change_function = std::function<void(rtc::PeerConnection::GatheringState state)>{
      std::bind(&Streamer::on_peer_gathering_state_change, this, std::placeholders::_1)};
  peer->connection->onStateChange(on_peer_state_change_function);
  peer->connection->onGatheringStateChange(on_peer_gathering_state_change_function);

  peer->data_channel = peer->connection->createDataChannel(DATA_CHANNEL_ID);

  auto on_data_channel_open_function = std::function<void()>{std::bind(&Streamer::on_data_channel_open, this)};
  auto on_data_channel_closed_function = std::function<void()>{std::bind(&Streamer::on_data_channel_closed, this)};
  auto on_data_channel_error_function =
      std::function<void(std::string error)>{std::bind(&Streamer::on_data_channel_error, this, std::placeholders::_1)};
  auto on_data_channel_binary_message_function = std::function<void(rtc::binary message)>{
      std::bind(&Streamer::on_data_channel_binary_message, this, std::placeholders::_1)};
  auto on_data_channel_string_message_function = std::function<void(std::string message)>{
      std::bind(&Streamer::on_data_channel_string_message, this, std::placeholders::_1)};

  peer->data_channel->onOpen(on_data_channel_open_function);
  peer->data_channel->onClosed(on_data_channel_closed_function);
  peer->data_channel->onError(on_data_channel_error_function);
  peer->data_channel->onMessage(on_data_channel_binary_message_function, on_data_channel_string_message_function);

  peer->connection->setLocalDescription();

  return peer;
}

void Streamer::send_video_stream_info() {
  if (!connection_open_) { return; }

  const auto video_stream_info_json = nlohmann::json{
      {     "width",      video_stream_info_.width},
      {    "height",     video_stream_info_.height},
      {       "fps",        video_stream_info_.fps},
      {  "codec_id",   video_stream_info_.codec_id},
      {"codec_name", video_stream_info_.codec_name}
  };
  const auto json = nlohmann::json{
      {"video_stream_info", video_stream_info_json}
  };
  web_socket_->send(json.dump());
}

void Streamer::video_stream_callback(const std::byte *data, const std::size_t size) {
  if (peer_ && peer_->data_channel && peer_->data_channel->isOpen()) { peer_->data_channel->send(data, size); }
}
} // namespace streaming
