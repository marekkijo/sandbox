#include "my_lib/my_test.hpp"
#include <rtc/rtc.hpp>

int main() {
  my_namespace::test();

  rtc::WebSocket web_socket;
  web_socket.onMessage([](std::vector<std::byte> data) {}, nullptr);

  return 0;
}
