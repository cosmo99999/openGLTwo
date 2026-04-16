
import os
from conan import ConanFile
from conan.tools.cmake import cmake_layout
from conan.tools.files import copy
class OpenGLProject(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires("opengl/system", libs=False, transitive_headers=True)
        self.requires("enet/1.3.18")
        self.requires("glfw/3.4")
        self.requires("glm/1.0.1")
        self.requires("imgui/1.92.6")
        self.requires("glad/0.1.36")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        copy(self, "*glfw*", os.path.join(self.dependencies["imgui"].package_folder,
            "res", "bindings"), os.path.join(self.source_folder, "bindings"))
        copy(self, "*opengl3*", os.path.join(self.dependencies["imgui"].package_folder,
            "res", "bindings"), os.path.join(self.source_folder, "bindings"))
        copy(self, "*imgui*", os.path.join(self.dependencies["imgui"].package_folder,
            "res", "src"), os.path.join(self.source_folder, "bindings"))
