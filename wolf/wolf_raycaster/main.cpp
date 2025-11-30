#include "raycaster_scene.hpp"

#include "wolf_common/raw_map_from_wolf.hpp"

#include <cstddef>
#include <gp/utils/utils.hpp>

#include <boost/program_options.hpp>

#include <iostream>
#include <memory>
#include <string>

constexpr auto MAPHEAD = "data/MAPHEAD.WL6";
constexpr auto GAMEMAPS = "data/GAMEMAPS.WL6";

struct ProgramSetup {
  bool exit{};

  int width{};
  int height{};
  int fov{};
  int num_rays{};
  std::size_t map_index{0u};
};

ProgramSetup process_args(const int argc, const char *const argv[]) {
  boost::program_options::options_description desc("Wolf options");
  desc.add_options()("help", "This help message");
  desc.add_options()("width", boost::program_options::value<int>()->default_value(1280), "Width of the window");
  desc.add_options()("height", boost::program_options::value<int>()->default_value(960), "Height of the window");
  desc.add_options()("fov", boost::program_options::value<int>()->default_value(60), "Field of view in degrees");
  desc.add_options()("num_rays", boost::program_options::value<int>()->default_value(320), "Number of rays to cast");
  desc.add_options()("map_index",
                     boost::program_options::value<std::size_t>()->default_value(0u),
                     "Index of the map to load");

  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);

  if (vm.count("help")) {
    desc.print(std::cout);
    return {true};
  }

  return {false,
          vm["width"].as<int>(),
          vm["height"].as<int>(),
          vm["fov"].as<int>(),
          vm["num_rays"].as<int>(),
          vm["map_index"].as<std::size_t>()};
}

int main(int argc, char *argv[]) {
  gp::utils::set_working_directory();

  const auto program_setup = process_args(argc, argv);
  if (program_setup.exit) {
    return 1;
  }

  auto raw_map_from_wolf = wolf::RawMapFromWolf{MAPHEAD, GAMEMAPS};
  auto raw_map = raw_map_from_wolf.create_map(program_setup.map_index);

  auto scene = std::make_unique<wolf::RaycasterScene>(std::move(raw_map), program_setup.fov, program_setup.num_rays);

  scene->init(program_setup.width, program_setup.height, "Wolf: Raycaster");

  return scene->exec();
}
