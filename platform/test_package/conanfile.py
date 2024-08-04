import os

from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.build import can_run


class cppelib_platformTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires(self.tested_reference_str)
        self.requires("cppelib_mechanism/[>=1.6.0]")

    def build(self):
        cmake = CMake(self)
        cmake.configure(variables={"CONAN_SETTINGS_OS": self.settings.os})
        cmake.build()

    def layout(self):
        cmake_layout(self)

    def test(self):
        if can_run(self):
            cmd = os.path.join(self.cpp.build.bindir, "PackageTest")
            self.run(cmd, env="conanrun")
