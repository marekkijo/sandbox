from conan import ConanFile
from conan.tools.cmake import cmake_layout


class SandboxRecipe(ConanFile):
    settings = 'os', 'compiler', 'build_type', 'arch'
    generators = 'CMakeToolchain', 'CMakeDeps'

    def requirements(self):
        self.requires('assimp/5.2.2')
        self.requires('boost/1.84.0')
        self.requires('ffmpeg/6.1')
        self.requires('glad/0.1.36')
        self.requires('glfw/3.3.8')
        self.requires('glm/0.9.9.8')
        self.requires('libdatachannel/0.20.2')
        self.requires('nlohmann_json/3.11.3')
        self.requires('sdl/2.28.5')
        self.requires('cgltf/1.13')

    def configure(self):
        self.options['boost'].without_cobalt = True

    def layout(self):
        cmake_layout(self)
