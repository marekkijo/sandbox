#include "server.hpp"

namespace streaming {
Server::Server(std::uint16_t port)
    : server_{
          rtc::WebSocketServer::Configuration{port, false, {}, {}, {}}
} {
  auto on_client_function =
      std::function<void(std::shared_ptr<rtc::WebSocket>)>{std::bind(&Server::on_client, this, std::placeholders::_1)};
  server_.onClient(on_client_function);
  printf("Listening on port: %i\n", port);
}

void Server::on_client(std::shared_ptr<rtc::WebSocket> incoming) {
  auto client        = std::make_shared<Client>();
  client->web_socket = incoming;

  if (auto remote_address = client->web_socket->remoteAddress()) {
    printf("Client connection received: %s\n", remote_address->c_str());
    init_client(client);
    temporary_store_.insert(client);
  }
}

void Server::init_client(std::shared_ptr<Client> &client) {
  auto on_client_open_function =
      std::function<void()>{std::bind(&Server::on_client_open, this, std::weak_ptr<Client>(client))};
  auto on_client_closed_function =
      std::function<void()>{std::bind(&Server::on_client_closed, this, std::weak_ptr<Client>(client))};
  auto on_client_error_function = std::function<void(std::string error)>{
      std::bind(&Server::on_client_error, this, std::weak_ptr<Client>(client), std::placeholders::_1)};
  auto on_client_binary_message_function = std::function<void(rtc::binary message)>{
      std::bind(&Server::on_client_binary_message, this, std::weak_ptr<Client>(client), std::placeholders::_1)};
  auto on_client_string_message_function = std::function<void(std::string message)>{
      std::bind(&Server::on_client_string_message, this, std::weak_ptr<Client>(client), std::placeholders::_1)};

  client->web_socket->onOpen(on_client_open_function);
  client->web_socket->onClosed(on_client_closed_function);
  client->web_socket->onError(on_client_error_function);
  client->web_socket->onMessage(on_client_binary_message_function, on_client_string_message_function);
}

void Server::on_client_open(std::weak_ptr<Client> weak_client) {
  auto client = weak_client.lock();
  temporary_store_.erase(client);

  if (auto remote_address = client->web_socket->remoteAddress()) {
    printf("Client connection opened: %s\n", remote_address->c_str());

    if (auto path = client->web_socket->path()) {
      client->info.parse(*path);

      switch (client->info.type()) {
      case Client::Type::unknown: printf("Unknown client connected\n  ignoring... connection will be closed\n"); break;
      case Client::Type::streamer:
        printf("Streamer '%s' connected\n", client->info.id().c_str());
        clients_[client->info.id()]      = client;
        streamers_[client->info.id()].id = client->info.id();
        break;
      case Client::Type::receiver:
        printf("Receiver '%s' connected\n", client->info.id().c_str());
        clients_[client->info.id()]      = client;
        receivers_[client->info.id()].id = client->info.id();
        break;
      }
    }
  }
}

void Server::on_client_closed(std::weak_ptr<Client> weak_client) {
  auto type = Client::Type::unknown;

  auto client = weak_client.lock();
  if (client) {
    clients_.erase(client->info.id());
    type = client->info.type();
  }

  switch (type) {
  case Client::Type::unknown: printf("Unknown client disconnected\n"); break;
  case Client::Type::streamer:
    printf("Streamer '%s' disconnected\n", client->info.id().c_str());
    streamers_.erase(client->info.id());
    break;
  case Client::Type::receiver:
    printf("Receiver '%s' disconnected\n", client->info.id().c_str());
    clients_.erase(client->info.id());
    receivers_.erase(client->info.id());
    break;
  }
}

void Server::on_client_error(std::weak_ptr<Client> weak_client, std::string error) {
  auto client = weak_client.lock();

  switch (client->info.type()) {
  case Client::Type::unknown: printf("Unknown client error: %s\n", error.c_str()); break;
  case Client::Type::streamer: printf("Streamer '%s' error: %s\n", client->info.id().c_str(), error.c_str()); break;
  case Client::Type::receiver: printf("Receiver '%s' error: %s\n", client->info.id().c_str(), error.c_str()); break;
  }
}

void Server::on_client_binary_message(std::weak_ptr<Client> /* weak_client */, rtc::binary /* message */) {
  printf("Received binary message\n");
}

void Server::on_client_string_message(std::weak_ptr<Client> weak_client, std::string message) {
  auto client = weak_client.lock();
  auto json   = nlohmann::json::parse(message);

  if (json.contains("video_stream_info")) {
    parse_video_stream_info(client, json.at("video_stream_info"));
    return;
  }

  if (json.contains("command")) {
    parse_command(client, json.at("command"));
    return;
  }

  if (json.contains("id") && json.contains("type") && json.contains("sdp")) {
    std::string id   = json.at("id");
    std::string type = json.at("type");
    std::string sdp  = json.at("sdp");

    if (type == "offer") {
      auto response_json = nlohmann::json{
          {  "id", client->info.id()},
          {"type",              type},
          { "sdp",               sdp}
      };
      clients_[id]->web_socket->send(response_json.dump());
      return;
    } else if (type == "answer") {
      auto response_json = nlohmann::json{
          {  "id", client->info.id()},
          {"type",              type},
          { "sdp",               sdp}
      };
      clients_[id]->web_socket->send(response_json.dump());
      return;
    }
  }

  printf("Unknown message: %s\n", message.c_str());
}

void Server::parse_video_stream_info(std::shared_ptr<Client> &client, const nlohmann::json &json_video_stream_info) {
  auto &video_stream_info      = streamers_[client->info.id()].video_stream_info;
  video_stream_info.width      = json_video_stream_info.at("width");
  video_stream_info.height     = json_video_stream_info.at("height");
  video_stream_info.fps        = json_video_stream_info.at("fps");
  video_stream_info.codec_id   = json_video_stream_info.at("codec_id");
  video_stream_info.codec_name = json_video_stream_info.at("codec_name");
}

void Server::parse_command(std::shared_ptr<Client> &client, const nlohmann::json &json_command) {
  const auto type = std::string{json_command.at("type")};
  if (type == "request_video_stream_infos") {
    auto video_stream_infos_json = nlohmann::json::array();
    for (const auto &streamer_pair : streamers_) {
      const auto &streamer = streamer_pair.second;
      if (!streamer.paired) {
        const auto video_stream_info_json = nlohmann::json{
            {"streamer_id",                           streamer.id},
            {      "width",      streamer.video_stream_info.width},
            {     "height",     streamer.video_stream_info.height},
            {        "fps",        streamer.video_stream_info.fps},
            {   "codec_id",   streamer.video_stream_info.codec_id},
            { "codec_name", streamer.video_stream_info.codec_name}
        };
        video_stream_infos_json.push_back(video_stream_info_json);
      }
    }
    const auto json = nlohmann::json{
        {"video_stream_infos", video_stream_infos_json}
    };
    client->web_socket->send(json.dump());
    return;
  } else if (type == "request_video_stream") {
    const auto streamer = std::string{json_command.at("streamer")};
    const auto receiver = std::string{json_command.at("receiver")};

    const auto request_video_stream_json = nlohmann::json{
        {"streamer", streamer},
        {"receiver", receiver}
    };
    const auto json = nlohmann::json{
        {"request_video_stream", request_video_stream_json}
    };
    clients_[streamer]->web_socket->send(json.dump());
    return;
  }
  printf("Unknown command: %s\n", json_command.dump().c_str());
}
} // namespace streaming
