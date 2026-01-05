from conan import ConanFile
from conan.tools.cmake import cmake_layout


class CompressorRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        if self.settings.os == "Linux":
            self.requires("gtk/system")
        self.requires("asio/1.36.0")
        self.requires("spdlog/1.16.0",)
        self.requires("glm/1.0.1")
        self.requires("imgui/1.92.5-docking")
        self.requires("raylib/5.5")
    
    def layout(self):
        cmake_layout(self)
