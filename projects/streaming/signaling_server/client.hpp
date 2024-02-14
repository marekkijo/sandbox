#pragma once

#include <rtc/rtc.hpp>

#include <cstdint>
#include <memory>
#include <string>

namespace streaming {
struct Client {
  enum class Type : std::uint8_t {
    unknown,
    streamer,
    receiver,
  };

  class Info {
  public:
    Info(const Info &) = delete;
    Info &operator=(const Info &) = delete;
    Info(Info &&other) noexcept = delete;
    Info &operator=(Info &&other) noexcept = delete;

    explicit Info(std::string path = "");

    [[nodiscard]] Type type() const;
    [[nodiscard]] const std::string &id() const;

    void parse(std::string path);

  private:
    Type type_{Type::unknown};
    std::string id_{};
  };

  std::shared_ptr<rtc::WebSocket> web_socket{};
  Info info{};
};
} // namespace streaming
