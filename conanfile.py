from conan import ConanFile
from conan.tools.cmake import cmake_layout


class CompressorRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("spdlog/1.16.0",)
        self.requires("glm/1.0.1")
        self.requires("imgui/1.92.5-docking")
        self.requires("boost/1.90.0")
        self.requires("raylib/5.5")
    
    def layout(self):
        cmake_layout(self)
