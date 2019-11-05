import os

from conans import ConanFile, CMake, tools


class LibtcodTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"

    def build(self):
        cmake = CMake(self)
        # Current dir is "test_package/build/<build_id>"
        cmake.configure()
        cmake.build()

    def imports(self):
        self.copy("*.dll", dst="bin", src="bin")
        self.copy("*.dylib*", dst="bin", src="lib")
        self.copy('*.so*', dst='bin', src='lib')

    def test(self):
        os.environ["DATA_DIR"] = os.path.join(self.source_folder, "../data")
        if not tools.cross_building(self.settings):
            os.chdir("bin")
            self.run(".%sunittest" % os.sep)
