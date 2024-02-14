#include "decoder.hpp"
#include "player.hpp"
#include "receiver.hpp"

#include <boost/program_options.hpp>

#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std::literals::chrono_literals;

struct ProgramSetup {
  bool exit{};

  std::string   ip{};
  std::uint16_t port{};
};

ProgramSetup process_args(const int argc, const char *const argv[]) {
  boost::program_options::options_description desc("Options");
  desc.add_options()("help", "This help message");
  desc.add_options()("ip", boost::program_options::value<std::string>()->default_value("127.0.0.1"), "Server ip");
  desc.add_options()("port", boost::program_options::value<std::uint16_t>()->default_value(11100u), "Server port");

  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);

  if (vm.count("help")) {
    desc.print(std::cout);
    return {true};
  }

  return {false, vm["ip"].as<std::string>(), vm["port"].as<std::uint16_t>()};
}

auto should_exit = std::atomic_bool{false};
auto mutex       = std::mutex{};

void wait_for_exit() {
  printf("Press 'enter' to exit\n");
  std::cin.ignore();
  printf("exiting...\n");
  {
    auto unique_lock = std::unique_lock{mutex};
    should_exit      = true;
  }
}

int main(int argc, char *argv[]) {
  const auto program_setup = process_args(argc, argv);
  if (program_setup.exit) { return 1; }

  try {
    auto                decoder = std::make_shared<streaming::Decoder>();
    auto                player  = std::make_shared<streaming::Player>(decoder);
    streaming::Receiver receiver(program_setup.ip, program_setup.port, decoder);
    std::thread         wait_thread(wait_for_exit);
    while (true) {
      std::this_thread::sleep_for(1000ms);
      auto unique_lock = std::unique_lock{mutex};
      if (should_exit) { break; }
    }
    wait_thread.join();
  } catch (const std::exception &e) {
    printf("Error: %s\n", e.what());
    return -1;
  }
}
