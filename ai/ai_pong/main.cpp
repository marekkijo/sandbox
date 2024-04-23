#include "pong_scene.hpp"

#include <boost/program_options.hpp>

#include <iostream>
#include <memory>
#include <string>

struct ProgramSetup {
  bool exit{};

  int width{};
  int height{};
};

ProgramSetup process_args(const int argc, const char *const argv[]) {
  boost::program_options::options_description desc("Wolf options");
  desc.add_options()("help", "This help message");
  desc.add_options()("width", boost::program_options::value<int>()->default_value(1024), "Width of the window");
  desc.add_options()("height", boost::program_options::value<int>()->default_value(1024), "Height of the window");

  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);

  if (vm.count("help")) {
    desc.print(std::cout);
    return {true};
  }

  return {false, vm["width"].as<int>(), vm["height"].as<int>()};
}

int main(int argc, char *argv[]) {
  const auto program_setup = process_args(argc, argv);
  if (program_setup.exit) {
    return 1;
  }

  auto scene = std::make_unique<ai::PongScene>();
  scene->init(program_setup.width, program_setup.height, "AI: Pong");
  return scene->exec();
}
