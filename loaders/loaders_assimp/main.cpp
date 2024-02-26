#include "model.hpp"
#include "model_scene.hpp"

#include <boost/program_options.hpp>

#include <iostream>
#include <memory>

struct ProgramSetup {
  bool exit{};

  std::string filename{};
  int width{};
  int height{};
  std::uint16_t fps{};
};

ProgramSetup process_args(const int argc, const char *const argv[]) {
  boost::program_options::options_description desc("Options");
  desc.add_options()("help", "This help message");
  desc.add_options()("filename",
                     boost::program_options::value<std::string>()->default_value("model.glb"),
                     "Model file name");
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
          vm["filename"].as<std::string>(),
          vm["width"].as<int>(),
          vm["height"].as<int>(),
          vm["fps"].as<std::uint16_t>()};
}

int main(int argc, char *argv[]) {
  const auto program_setup = process_args(argc, argv);
  if (program_setup.exit) { return 1; }

    auto model = std::make_shared<const Model>(program_setup.filename);
  auto model_scene = std::make_unique<ModelScene>(model);
  model_scene->set_fps(program_setup.fps);
  model_scene->init(program_setup.width, program_setup.height, "loaders_assimp");
  return model_scene->exec();
}
