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
          vm["fps"].as<std::uint16_t>()};
}

int main(int argc, char *argv[]) {
  const auto program_setup = process_args(argc, argv);
  if (program_setup.exit) {
    return 1;
  }

  printf("ip: %s\n", program_setup.ip.c_str());
  printf("port: %d\n", program_setup.port);
  printf("width: %d\n", program_setup.width);
  printf("height: %d\n", program_setup.height);
  printf("fps: %d\n", program_setup.fps);

  return 0;
}
