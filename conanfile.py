from conan import ConanFile

class SandboxRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("boost/1.84.0")
        #self.requires("ffmpeg/6.1")
        #self.requires("glew/2.2.0")
        self.requires("glm/0.9.9.8")
        #self.requires("libdatachannel/0.20.1")
        #self.requires("nlohmann_json/3.11.3")
        self.requires("sdl/2.28.5")

#    def configure(self):
#        self.options['boost'].shared = True
#        self.options['ffmpeg'].shared = True
#        self.options['glew'].shared = True
#        self.options['sdl'].shared = True
#        self.options['zlib'].shared = True
