#include "decode.hpp"
#include "encode.hpp"

#include <gp/ffmpeg/misc.hpp>

#include <boost/program_options.hpp>

#include <iostream>

struct ProgramSetup {
  bool exit{};

  int width{};
  int height{};
  std::uint16_t fps{};
  AVCodecID codec_id{AV_CODEC_ID_NONE};
  int length_s{};
};

ProgramSetup process_args(const int argc, const char **const argv) {
  boost::program_options::options_description desc("Options");
  desc.add_options()("help", "This help message");
  desc.add_options()("width", boost::program_options::value<int>()->default_value(512), "Width of the frame buffer");
  desc.add_options()("height", boost::program_options::value<int>()->default_value(384), "Height of the frame buffer");
  desc.add_options()("fps",
                     boost::program_options::value<std::uint16_t>()->default_value(30u),
                     "Number of frames per second");
  desc.add_options()("codec",
                     boost::program_options::value<std::string>()->default_value("h264"),
                     "Codec name, e.g. h264 or mpeg4");
  desc.add_options()("length_s",
                     boost::program_options::value<int>()->default_value(3),
                     "Length of the stream in seconds");

  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);

  if (vm.count("help") != 0U) {
    desc.print(std::cout);
    return {true};
  }

  return {false,
          vm["width"].as<int>(),
          vm["height"].as<int>(),
          vm["fps"].as<std::uint16_t>(),
          gp::ffmpeg::codec_name_to_id(vm["codec"].as<std::string>()),
          vm["length_s"].as<int>()};
}

int main(int argc, const char **argv) {
  const auto program_setup = process_args(argc, argv);
  if (program_setup.exit) { return 1; }

  const auto video_stream_info = streaming::VideoStreamInfo{program_setup.width,
                                                            program_setup.height,
                                                            program_setup.fps,
                                                            program_setup.codec_id,
                                                            avcodec_get_name(program_setup.codec_id)};

  streaming::encode(video_stream_info, program_setup.length_s);
  streaming::decode(video_stream_info);

  return 0;
}
