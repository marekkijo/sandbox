#include "single_thread_wolf_scene.hpp"

#include "wolf_common/raw_map_from_wolf.hpp"

#include <gp/utils/utils.hpp>

#include <boost/program_options.hpp>

#include <iostream>
#include <memory>
#include <string>

struct ProgramSetup {
  bool exit{};

  std::string asciimap{};
  std::string maphead{};
  std::string gamemaps{};
  int width{};
  int height{};
  std::uint32_t fov{};
  std::uint32_t rays{};
};

ProgramSetup process_args(const int argc, const char *const argv[]) {
  boost::program_options::options_description desc("Wolf options");
  desc.add_options()("help", "This help message");
  desc.add_options()("asciimap",
                     boost::program_options::value<std::string>()->default_value("data/map1.map"),
                     "Ascii map filename");
  desc.add_options()("maphead",
                     boost::program_options::value<std::string>()->default_value("data/MAPHEAD.WL6"),
                     "Wolf MAPHEAD filename");
  desc.add_options()("gamemaps",
                     boost::program_options::value<std::string>()->default_value("data/GAMEMAPS.WL6"),
                     "Wolf GAMEMAPS filename");
  desc.add_options()("width", boost::program_options::value<int>()->default_value(1520), "Width of the window");
  desc.add_options()("height", boost::program_options::value<int>()->default_value(760), "Height of the window");
  desc.add_options()("fov",
                     boost::program_options::value<std::uint32_t>()->default_value(60u),
                     "Field of view in degrees");
  desc.add_options()("rays", boost::program_options::value<std::uint32_t>()->default_value(152u), "Number of rays");

  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);

  if (vm.count("help")) {
    desc.print(std::cout);
    return {true};
  }

  return {false,
          vm["asciimap"].as<std::string>(),
          vm["maphead"].as<std::string>(),
          vm["gamemaps"].as<std::string>(),
          vm["width"].as<int>(),
          vm["height"].as<int>(),
          vm["fov"].as<std::uint32_t>(),
          vm["rays"].as<std::uint32_t>()};
}

std::unique_ptr<gp::sdl::Scene2D> create_scene(const ProgramSetup &program_setup) {
  auto raw_map_from_wolf = wolf::RawMapFromWolf{program_setup.maphead, program_setup.gamemaps};
  const auto raw_map = raw_map_from_wolf.create_map(0u);

  auto scene = std::make_unique<wolf::SingleThreadWolfScene>(raw_map, program_setup.fov, program_setup.rays);
  scene->init(program_setup.width, program_setup.height, "Wolf: Singlethread");
  return scene;
}

int main(int argc, char *argv[]) {
  gp::utils::set_working_directory();

  const auto program_setup = process_args(argc, argv);
  if (program_setup.exit) {
    return 1;
  }

  const auto scene = create_scene(program_setup);
  return scene->exec();
}
