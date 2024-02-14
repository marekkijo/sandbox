#pragma once

#include "common/common.hpp"

#include "client.hpp"

#include <nlohmann/json.hpp>
#include <rtc/rtc.hpp>

#include <memory>
#include <unordered_set>

namespace streaming {
struct StreamerInfo {
  std::string     id{};
  bool            paired{false};
  VideoStreamInfo video_stream_info{};
};

struct ReceiverInfo {
  std::string id{};
  bool        paired{false};
};

class Server {
public:
  Server(const Server &)                     = delete;
  Server &operator=(const Server &)          = delete;
  Server(Server &&other) noexcept            = delete;
  Server &operator=(Server &&other) noexcept = delete;

  explicit Server(std::uint16_t port);

private:
  void on_client(std::shared_ptr<rtc::WebSocket> incoming);

  void init_client(std::shared_ptr<Client> &client);

  void on_client_open(std::weak_ptr<Client> client);
  void on_client_closed(std::weak_ptr<Client> client);
  void on_client_error(std::weak_ptr<Client> client, std::string error);
  void on_client_binary_message(std::weak_ptr<Client> client, rtc::binary message);
  void on_client_string_message(std::weak_ptr<Client> client, std::string message);

  void parse_video_stream_info(std::shared_ptr<Client> &client, const nlohmann::json &json_video_stream_info);
  void parse_command(std::shared_ptr<Client> &client, const nlohmann::json &json_command);

  rtc::WebSocketServer                                     server_{};
  std::unordered_set<std::shared_ptr<Client>>              temporary_store_{};
  std::unordered_map<std::string, std::shared_ptr<Client>> clients_{};
  std::unordered_map<std::string, StreamerInfo>            streamers_{};
  std::unordered_map<std::string, ReceiverInfo>            receivers_{};
};
} // namespace streaming
