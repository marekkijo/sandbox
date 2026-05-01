#include "streamer.hpp"

#include "streaming_common/constants.hpp"
#include "streaming_common/encoder.hpp"

#include <gp/json/misc.hpp>
#include <gp/utils/utils.hpp>

namespace streaming {
Streamer::Streamer(const std::string &server_ip, const std::uint16_t server_port)
    : id_{std::string{STREAMER_ID} + ":" + gp::utils::generate_random_string(16u)}
    , connection_url_{std::string{"ws://"} + server_ip + ":" + std::to_string(server_port) + "/" + id_} {
  configuration_.iceServers.emplace_back("stun.l.google.com", 19302);
  configuration_.disableAutoNegotiation = true;
  configuration_.maxMessageSize = MAX_MESSAGE_SIZE;

  web_socket_ = std::make_shared<rtc::WebSocket>();
  printf("Connection url: %s\n", connection_url_.c_str());
}

void Streamer::start(std::shared_ptr<Encoder> encoder) {
  video_stream_info_ = encoder->video_stream_info();
  init_web_socket(web_socket_);
  auto weak_self = weak_from_this();
  encoder->set_video_stream_callback([weak_self](const std::byte *data, const std::size_t size, const bool eof) {
    if (auto self = weak_self.lock()) {
      self->video_stream_callback(data, size, eof);
    }
  });
  web_socket_->open(connection_url_);
}

void Streamer::set_event_callback(std::function<void(const gp::misc::Event &event)> event_callback) {
  event_callback_ = std::move(event_callback);
}

void Streamer::init_web_socket(std::shared_ptr<rtc::WebSocket> web_socket) {
  auto weak_self = weak_from_this();
  web_socket->onOpen([weak_self]() {
    if (auto self = weak_self.lock()) {
      self->on_web_socket_open();
    }
  });
  web_socket->onClosed([weak_self]() {
    if (auto self = weak_self.lock()) {
      self->on_web_socket_closed();
    }
  });
  web_socket->onError([weak_self](std::string error) {
    if (auto self = weak_self.lock()) {
      self->on_web_socket_error(std::move(error));
    }
  });
  web_socket->onMessage(
      [weak_self](rtc::binary message) {
        if (auto self = weak_self.lock()) {
          self->on_web_socket_binary_message(std::move(message));
        }
      },
      [weak_self](std::string message) {
        if (auto self = weak_self.lock()) {
          self->on_web_socket_string_message(std::move(message));
        }
      });
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
  try {
    auto json = nlohmann::json::parse(message);

    if (json.contains("id") && json.contains("type") && json.contains("sdp")) {
      std::string id = json.at("id");
      std::string type = json.at("type");
      std::string sdp = json.at("sdp");

      if (type == "answer") {
        std::shared_ptr<Peer> peer;
        {
          std::lock_guard<std::mutex> lock(mutex_);
          peer = peer_;
        }
        if (peer && id == peer->id) {
          auto description = rtc::Description{sdp, type};
          peer->connection->setRemoteDescription(description);
        }
        return;
      }
    } else if (json.contains("request_video_stream")) {
      std::shared_ptr<Peer> existing_peer;
      {
        std::lock_guard<std::mutex> lock(mutex_);
        existing_peer = peer_;
      }
      if (existing_peer) {
        return;
      }

      std::string streamer = json.at("request_video_stream").at("streamer");
      std::string receiver = json.at("request_video_stream").at("receiver");

      auto new_peer = create_peer(receiver);
      {
        std::lock_guard<std::mutex> lock(mutex_);
        peer_ = new_peer;
      }
      return;
    }

    printf("Unknown message: %s\n", message.c_str());
  } catch (const std::exception &e) {
    printf("Error processing message: %s\n", e.what());
  }
}

void Streamer::on_data_channel_open() { printf("Data channel opened\n"); }

void Streamer::on_data_channel_closed() { printf("Data channel closed\n"); }

void Streamer::on_data_channel_error(std::string error) { printf("Data channel error: %s\n", error.c_str()); }

void Streamer::on_data_channel_binary_message(rtc::binary /* message */) {
  printf("Received data channel binary message\n");
}

void Streamer::on_data_channel_string_message(std::string message) {
  try {
    auto json = nlohmann::json::parse(message);

    if (json.contains("event")) {
      parse_event(json.at("event"));
      return;
    }

    printf("Unknown message: %s\n", message.c_str());
  } catch (const std::exception &e) {
    printf("Error processing data channel message: %s\n", e.what());
  }
}

void Streamer::on_peer_state_change(rtc::PeerConnection::State state) {
  switch (state) {
  case rtc::PeerConnection::State::Connecting:
    printf("Peer state: Connecting\n");
    break;
  case rtc::PeerConnection::State::Connected:
    printf("Peer state: Connected\n");
    break;
  case rtc::PeerConnection::State::Disconnected: {
    printf("Peer state: Disconnected\n");
    std::lock_guard<std::mutex> lock(mutex_);
    peer_ = nullptr;
    break;
  }
  case rtc::PeerConnection::State::Failed: {
    printf("Peer state: Failed\n");
    std::lock_guard<std::mutex> lock(mutex_);
    peer_ = nullptr;
    break;
  }
  case rtc::PeerConnection::State::Closed: {
    printf("Peer state: Closed\n");
    std::lock_guard<std::mutex> lock(mutex_);
    peer_ = nullptr;
    break;
  }

  default:
    break;
  }
}

void Streamer::on_peer_gathering_state_change(rtc::PeerConnection::GatheringState state) {
  if (state == rtc::PeerConnection::GatheringState::Complete) {
    std::shared_ptr<Peer> peer;
    {
      std::lock_guard<std::mutex> lock(mutex_);
      peer = peer_;
    }
    if (!peer) {
      return;
    }
    auto description = peer->connection->localDescription();
    const auto json = nlohmann::json{
        {  "id",                         peer->id},
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

  auto weak_self = weak_from_this();
  peer->connection->onStateChange([weak_self](rtc::PeerConnection::State state) {
    if (auto self = weak_self.lock()) {
      self->on_peer_state_change(state);
    }
  });
  peer->connection->onGatheringStateChange([weak_self](rtc::PeerConnection::GatheringState state) {
    if (auto self = weak_self.lock()) {
      self->on_peer_gathering_state_change(state);
    }
  });

  peer->data_channel = peer->connection->createDataChannel(DATA_CHANNEL_ID);

  peer->data_channel->onOpen([weak_self]() {
    if (auto self = weak_self.lock()) {
      self->on_data_channel_open();
    }
  });
  peer->data_channel->onClosed([weak_self]() {
    if (auto self = weak_self.lock()) {
      self->on_data_channel_closed();
    }
  });
  peer->data_channel->onError([weak_self](std::string error) {
    if (auto self = weak_self.lock()) {
      self->on_data_channel_error(std::move(error));
    }
  });
  peer->data_channel->onMessage(
      [weak_self](rtc::binary message) {
        if (auto self = weak_self.lock()) {
          self->on_data_channel_binary_message(std::move(message));
        }
      },
      [weak_self](std::string message) {
        if (auto self = weak_self.lock()) {
          self->on_data_channel_string_message(std::move(message));
        }
      });

  peer->connection->setLocalDescription();

  return peer;
}

void Streamer::send_video_stream_info() {
  if (!connection_open_) {
    return;
  }

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

void Streamer::video_stream_callback(const std::byte *data, const std::size_t size, const bool eof) {
  std::shared_ptr<Peer> peer;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    peer = peer_;
  }
  if (peer && peer->data_channel && peer->data_channel->isOpen()) {
    peer->data_channel->send(data, size);
    if (eof) {
      constexpr auto eof_data = "EOF";
      constexpr auto eof_size = 4u;
      peer->data_channel->send(reinterpret_cast<const std::byte *>(eof_data), eof_size);
    }
  }
}

void Streamer::parse_event(const nlohmann::json &json_event) {
  const auto event = gp::json::to_event(json_event);
  if (event_callback_) {
    event_callback_(event);
  }
}
} // namespace streaming
