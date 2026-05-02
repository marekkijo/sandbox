#include "raycaster_scene.hpp"

#include "wolf_common/raw_map_from_wolf.hpp"
#include "wolf_common/vswap_file.hpp"

#include <cstddef>
#include <gp/utils/utils.hpp>

#include <boost/program_options.hpp>

#include <iostream>
#include <memory>
#include <string>

constexpr auto MAPHEAD = "data/MAPHEAD.WL6";
constexpr auto GAMEMAPS = "data/GAMEMAPS.WL6";
constexpr auto VSWAP = "data/VSWAP.WL6";

struct ProgramSetup {
  bool exit{};
  int return_code{};

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
  desc.add_options()("num_rays", boost::program_options::value<int>()->default_value(256), "Number of rays to cast");
  desc.add_options()("map_index",
                     boost::program_options::value<std::size_t>()->default_value(0u),
                     "Index of the map to load");

  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);

  if (vm.count("help")) {
    desc.print(std::cout);
    return {true, 0};
  }

  const auto program_setup = ProgramSetup{false,
                                          0,
                                          vm["width"].as<int>(),
                                          vm["height"].as<int>(),
                                          vm["fov"].as<int>(),
                                          vm["num_rays"].as<int>(),
                                          vm["map_index"].as<std::size_t>()};

  if (program_setup.fov <= 0 || program_setup.fov >= 180) {
    std::cerr << "Error: FOV must be in (0, 180) range" << std::endl;
    return {true, 1};
  }

  if (program_setup.num_rays <= 0) {
    std::cerr << "Error: Number of rays must be positive" << std::endl;
    return {true, 1};
  }

  if (program_setup.width <= 0 || program_setup.height <= 0) {
    std::cerr << "Error: Width and height must be positive" << std::endl;
    return {true, 1};
  }

  return program_setup;
}

int main(int argc, char *argv[]) {
  gp::utils::set_working_directory();

  const auto program_setup = process_args(argc, argv);
  if (program_setup.exit) {
    return program_setup.return_code;
  }

  auto raw_map_from_wolf = wolf::RawMapFromWolf{MAPHEAD, GAMEMAPS};
  auto raw_map = raw_map_from_wolf.create_map(program_setup.map_index);
  auto vswap_file = std::make_shared<const wolf::VswapFile>(VSWAP);

  auto scene =
      std::make_unique<wolf::RaycasterScene>(std::move(raw_map), vswap_file, program_setup.fov, program_setup.num_rays);

  scene->init(program_setup.width, program_setup.height, "Wolf: Raycaster");

  return scene->exec();
}
