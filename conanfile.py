from conan import ConanFile
from conan.tools.cmake import cmake_layout


class CompressorRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("glfw/3.4")
        self.requires("glm/1.0.1")
        self.requires("ktx/4.3.2")
        self.requires("imgui/1.92.5-docking")
        self.requires("boost/1.90.0")
    
    def layout(self):
        cmake_layout(self)
