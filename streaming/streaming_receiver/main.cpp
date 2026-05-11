#include "decode_scene.hpp"
#include "receiver.hpp"

#include <gp/utils/utils.hpp>

#include <boost/program_options.hpp>

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

struct ProgramSetup {
  bool exit{};

  std::string ip{};
  std::uint16_t port{};
#ifdef STREAMING_PIPELINE_STATS
  std::string stats_log{};
  uint32_t stats_reports{20};
#endif
};

ProgramSetup process_args(const int argc, const char *const argv[]) {
  boost::program_options::options_description desc("Options");
  desc.add_options()("help", "This help message");
  desc.add_options()("ip", boost::program_options::value<std::string>()->default_value("127.0.0.1"), "Server ip");
  desc.add_options()("port", boost::program_options::value<std::uint16_t>()->default_value(11100u), "Server port");
#ifdef STREAMING_PIPELINE_STATS
  desc.add_options()("stats-log",
                     boost::program_options::value<std::string>()->default_value(""),
                     "File path for pipeline stats log (empty = stdout)");
  desc.add_options()("stats-reports",
                     boost::program_options::value<uint32_t>()->default_value(20u),
                     "Number of stat reports before auto-close (0 = unlimited)");
#endif

  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);

  if (vm.count("help")) {
    desc.print(std::cout);
    return {true};
  }

#ifdef STREAMING_PIPELINE_STATS
  return {false,
          vm["ip"].as<std::string>(),
          vm["port"].as<std::uint16_t>(),
          vm["stats-log"].as<std::string>(),
          vm["stats-reports"].as<uint32_t>()};
#else
  return {false, vm["ip"].as<std::string>(), vm["port"].as<std::uint16_t>()};
#endif
}

int main(int argc, char *argv[]) {
  gp::utils::set_working_directory();

  const auto program_setup = process_args(argc, argv);
  if (program_setup.exit) {
    return 1;
  }

  auto decode_scene = std::make_unique<streaming::DecodeScene>();
  auto receiver = std::make_shared<streaming::Receiver>(program_setup.ip, program_setup.port);

  receiver->set_video_stream_info_callback(
      [&decode_scene](const streaming::VideoStreamInfo &video_stream_info) { decode_scene->init(video_stream_info); });
  receiver->set_incoming_video_stream_data_callback(
      [&decode_scene](const std::byte *data, const std::size_t size, const bool eof) {
        decode_scene->consume_data(data, size, eof);
      });
  decode_scene->set_event_callback([&receiver](const gp::misc::Event &event) { receiver->handle_event(event); });

#ifdef STREAMING_PIPELINE_STATS
  std::FILE *stats_file{nullptr};
  if (!program_setup.stats_log.empty()) {
    stats_file = std::fopen(program_setup.stats_log.c_str(), "w");
  }
  decode_scene->set_stats_log(stats_file != nullptr ? stats_file : stdout);
  decode_scene->set_max_stats_reports(program_setup.stats_reports);
#endif

  receiver->connect();

  const auto async_init = true;
  const auto result = decode_scene->exec(async_init);

#ifdef STREAMING_PIPELINE_STATS
  if (stats_file != nullptr) {
    std::fclose(stats_file);
  }
#endif

  return result;
}
