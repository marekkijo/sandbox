#include "client.hpp"

#include "streaming_common/common.hpp"

#include <gp/utils/utils.hpp>

namespace streaming {
Client::Info::Info(std::string path) { parse(path); }

void Client::Info::parse(std::string path) {
  auto path_parts = gp::utils::split_by(path, "/");
  if (path_parts.size() < 2) { return; }

  auto label_parts = gp::utils::split_by(path_parts[1], ":");
  if (label_parts.size() < 2) { return; }

  id_ = label_parts[1];
  if (label_parts[0] == STREAMER_ID) {
    type_ = Type::streamer;
  } else if (label_parts[0] == RECEIVER_ID) {
    type_ = Type::receiver;
  }
}

Client::Type Client::Info::type() const { return type_; }

const std::string &Client::Info::id() const { return id_; }
} // namespace streaming
