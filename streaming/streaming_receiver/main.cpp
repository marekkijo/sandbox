#include "decode_scene.hpp"
#include "receiver.hpp"

#include <boost/program_options.hpp>

#include <cstdint>
#include <iostream>
#include <string>

struct ProgramSetup {
  bool exit{};

  std::string ip{};
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

int main(int argc, char *argv[]) {
  const auto program_setup = process_args(argc, argv);
  if (program_setup.exit) { return 1; }

  auto decode_scene = std::make_unique<streaming::DecodeScene>();
  auto receiver = std::make_unique<streaming::Receiver>(program_setup.ip, program_setup.port);

  receiver->set_video_stream_info_callback(
      [&decode_scene](const streaming::VideoStreamInfo &video_stream_info) { decode_scene->init(video_stream_info); });
  receiver->set_incoming_video_stream_data_callback(
      [&decode_scene](const std::byte *data, const std::size_t size) { decode_scene->consume_data(data, size); });
  decode_scene->set_event_callback([&receiver](const gp::misc::Event &event) { receiver->handle_event(event); });

  receiver->connect();

  const auto async_init = true;
  return decode_scene->exec(async_init);
}
