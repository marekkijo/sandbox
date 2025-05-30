#include "sprite_voxelizer.hpp"
#include "voxel_scene.hpp"

#include "wolf_common/vswap_file.hpp"

#include <boost/program_options.hpp>

#include <iostream>
#include <memory>

struct ProgramSetup {
  bool exit{};

  int width{};
  int height{};
};

ProgramSetup process_args(const int argc, const char *const argv[]) {
  boost::program_options::options_description desc("Options");
  desc.add_options()("help", "This help message");
  desc.add_options()("width", boost::program_options::value<int>()->default_value(1024), "Width of the frame buffer");
  desc.add_options()("height", boost::program_options::value<int>()->default_value(768), "Height of the frame buffer");

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

  auto vswap_file = std::make_shared<const wolf::VswapFile>("data/VSWAP.WL6");
  auto sprite_voxelizer = std::make_shared<SpriteVoxelizer>(vswap_file);
  auto voxel_scene = std::make_unique<VoxelScene>(sprite_voxelizer);
  voxel_scene->init(program_setup.width, program_setup.height, "sprite_voxelizer");
  return voxel_scene->exec();
}
