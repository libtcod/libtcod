from conans import ConanFile, CMake


class LibtcodSamplesConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    requires = (
        "libtcod/[~=1.15]@hexdecimal/stable",
        "sdl2/[~=2.0.5]@bincrafters/stable",
    )
    default_options = {"*:shared": True}

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def imports(self):
        self.copy("*.dll", "bin", "bin")
        self.copy("*.pdb", "bin", "bin")
        self.copy("*.dylib", "bin", "lib")
