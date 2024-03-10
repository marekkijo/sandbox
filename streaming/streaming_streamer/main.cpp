#include "encode_scene.hpp"
#include "streamer.hpp"

#include "streaming_common/video_stream_info.hpp"

#include <gp/ffmpeg/misc.hpp>
#include <gp/misc/event.hpp>

#include <boost/program_options.hpp>

#include <cstdint>
#include <iostream>
#include <string>

struct ProgramSetup {
  bool exit{};

  std::string ip{};
  std::uint16_t port{};
  int width{};
  int height{};
  std::uint16_t fps{};

  AVCodecID codec_id{AV_CODEC_ID_NONE};
};

ProgramSetup process_args(const int argc, const char *const argv[]) {
  boost::program_options::options_description desc("Options");
  desc.add_options()("help", "This help message");
  desc.add_options()("ip", boost::program_options::value<std::string>()->default_value("127.0.0.1"), "Server ip");
  desc.add_options()("port", boost::program_options::value<std::uint16_t>()->default_value(11100u), "Server port");
  desc.add_options()("width", boost::program_options::value<int>()->default_value(1024), "Width of the frame buffer");
  desc.add_options()("height", boost::program_options::value<int>()->default_value(768), "Height of the frame buffer");
  desc.add_options()("fps",
                     boost::program_options::value<std::uint16_t>()->default_value(30u),
                     "Number of frames per second");
  desc.add_options()("codec",
                     boost::program_options::value<std::string>()->default_value("h264"),
                     "Codec name, e.g. h264 or mpeg4");

  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);

  if (vm.count("help")) {
    desc.print(std::cout);
    return {true};
  }

  return {false,
          vm["ip"].as<std::string>(),
          vm["port"].as<std::uint16_t>(),
          vm["width"].as<int>(),
          vm["height"].as<int>(),
          vm["fps"].as<std::uint16_t>(),
          gp::ffmpeg::codec_name_to_id(vm["codec"].as<std::string>())};
}

int main(int argc, char *argv[]) {
  const auto program_setup = process_args(argc, argv);
  if (program_setup.exit) { return 1; }

  const auto video_stream_info = streaming::VideoStreamInfo{program_setup.width,
                                                            program_setup.height,
                                                            program_setup.fps,
                                                            program_setup.codec_id,
                                                            avcodec_get_name(program_setup.codec_id)};

  auto encode_scene = std::make_unique<streaming::EncodeScene>(video_stream_info);
  auto streamer = std::make_unique<streaming::Streamer>(program_setup.ip, program_setup.port, encode_scene->encoder());

  streamer->set_event_callback([&encode_scene](const gp::misc::Event &event) { encode_scene->handle_event(event); });

  return encode_scene->exec();
}
