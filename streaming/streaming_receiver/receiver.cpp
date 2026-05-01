#include "receiver.hpp"

#include "streaming_common/constants.hpp"

#include <gp/json/misc.hpp>
#include <gp/utils/utils.hpp>

namespace streaming {
Receiver::Receiver(const std::string &server_ip, const std::uint16_t server_port)
    : receiver_id_{gp::utils::generate_random_string(16u)}
    , id_{std::string{RECEIVER_ID} + ":" + receiver_id_} {
  configuration_.maxMessageSize = MAX_MESSAGE_SIZE;

  web_socket_ = std::make_shared<rtc::WebSocket>();
  connection_url_ = std::string{"ws://"} + server_ip + ":" + std::to_string(server_port) + "/" + id_;
  printf("Connection url: %s\n", connection_url_.c_str());
}

void Receiver::connect() {
  init_web_socket(web_socket_);
  web_socket_->open(connection_url_);
}

void Receiver::handle_event(const gp::misc::Event &event) {
  std::shared_ptr<Peer> peer;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    peer = peer_;
  }
  if (peer && peer->data_channel && peer->data_channel->isOpen()) {
    const auto json_event = gp::json::from_event(event);
    const auto json = nlohmann::json{
        {"event", json_event}
    };
    peer->data_channel->send(json.dump());
  }
}

void Receiver::set_video_stream_info_callback(
    std::function<void(const VideoStreamInfo &video_stream_info)> video_stream_info_callback) {
  video_stream_info_callback_ = std::move(video_stream_info_callback);
}

void Receiver::set_incoming_video_stream_data_callback(
    std::function<void(const std::byte *data, const std::size_t size)> incoming_video_stream_data_callback) {
  incoming_video_stream_data_callback_ = std::move(incoming_video_stream_data_callback);
}

void Receiver::init_web_socket(std::shared_ptr<rtc::WebSocket> web_socket) {
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
  try {
    auto json = nlohmann::json::parse(message);

    if (json.contains("id") && json.contains("type") && json.contains("sdp")) {
      std::string id = json.at("id");
      std::string type = json.at("type");
      std::string sdp = json.at("sdp");

      if (type == "offer") {
        auto new_peer = create_peer(id);
        std::shared_ptr<Peer> peer;
        {
          std::lock_guard<std::mutex> lock(mutex_);
          peer_ = new_peer;
          peer = peer_;
        }
        auto description = rtc::Description{sdp, type};
        peer->connection->setRemoteDescription(description);
        return;
      }
    } else if (json.contains("video_stream_infos")) {
      parse_video_stream_infos(json.at("video_stream_infos"));
      return;
    }

    printf("Unknown message: %s\n", message.c_str());
  } catch (const std::exception &e) {
    printf("Error processing message: %s\n", e.what());
  }
}

void Receiver::on_peer_state_change(rtc::PeerConnection::State state) {
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

void Receiver::on_peer_gathering_state_change(rtc::PeerConnection::GatheringState state) {
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

void Receiver::on_peer_local_description(rtc::Description description) {
  std::shared_ptr<Peer> peer;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    peer = peer_;
  }
  if (!peer) {
    return;
  }
  const auto json = nlohmann::json{
      {         "id",                 peer->id},
      {       "type", description.typeString()},
      {"description", std::string(description)}
  };
  web_socket_->send(json.dump());
}

void Receiver::on_peer_local_candidate(rtc::Candidate candidate) {
  std::shared_ptr<Peer> peer;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    peer = peer_;
  }
  if (!peer) {
    return;
  }
  const auto json = nlohmann::json{
      {       "id",               peer->id},
      {     "type",            "candidate"},
      {"candidate", std::string(candidate)},
      {      "mid",        candidate.mid()}
  };
  web_socket_->send(json.dump());
}

void Receiver::on_peer_data_channel(std::shared_ptr<rtc::DataChannel> data_channel) {
  {
    std::lock_guard<std::mutex> lock(mutex_);
    peer_->data_channel = data_channel;
  }

  auto weak_self = weak_from_this();
  data_channel->onOpen([weak_self]() {
    if (auto self = weak_self.lock()) {
      self->on_data_channel_open();
    }
  });
  data_channel->onClosed([weak_self]() {
    if (auto self = weak_self.lock()) {
      self->on_data_channel_closed();
    }
  });
  data_channel->onError([weak_self](std::string error) {
    if (auto self = weak_self.lock()) {
      self->on_data_channel_error(std::move(error));
    }
  });
  data_channel->onMessage(
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
}

void Receiver::on_data_channel_open() { printf("Data channel opened\n"); }

void Receiver::on_data_channel_closed() { printf("Data channel closed\n"); }

void Receiver::on_data_channel_error(std::string error) { printf("Data channel error: %s\n", error.c_str()); }

void Receiver::on_data_channel_binary_message(rtc::binary message) {
  if (incoming_video_stream_data_callback_) {
    incoming_video_stream_data_callback_(message.data(), message.size());
  } else {
    printf("Incoming video stream data callback not set\n");
  }
}

void Receiver::on_data_channel_string_message(std::string /* message */) {
  printf("Received data channel string message\n");
}

std::shared_ptr<Receiver::Peer> Receiver::create_peer(const std::string &id) {
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
  peer->connection->onLocalDescription([weak_self](rtc::Description description) {
    if (auto self = weak_self.lock()) {
      self->on_peer_local_description(std::move(description));
    }
  });
  peer->connection->onLocalCandidate([weak_self](rtc::Candidate candidate) {
    if (auto self = weak_self.lock()) {
      self->on_peer_local_candidate(std::move(candidate));
    }
  });
  peer->connection->onDataChannel([weak_self](std::shared_ptr<rtc::DataChannel> data_channel) {
    if (auto self = weak_self.lock()) {
      self->on_peer_data_channel(std::move(data_channel));
    }
  });

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
  try {
    streamer_infos_.clear();
    for (auto it = json_video_stream_infos.begin(); it != json_video_stream_infos.end(); ++it) {
      streamer_infos_.emplace_back();
      streamer_infos_.back().streamer_id = it->at("streamer_id");
      streamer_infos_.back().video_stream_info.width = it->at("width");
      streamer_infos_.back().video_stream_info.height = it->at("height");
      streamer_infos_.back().video_stream_info.fps = it->at("fps");
      streamer_infos_.back().video_stream_info.codec_id = it->at("codec_id");
      streamer_infos_.back().video_stream_info.codec_name = it->at("codec_name");

      if (video_stream_info_callback_) {
        video_stream_info_callback_(streamer_infos_.back().video_stream_info);
      } else {
        throw std::runtime_error("Video stream info callback not set");
      }
      command_request_video_stream(streamer_infos_.back().streamer_id);
    }
  } catch (const std::exception &e) {
    printf("Error parsing video stream infos: %s\n", e.what());
  }
}
} // namespace streaming
