import os

from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.build import can_run


class cppelib_platform_testTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires(self.tested_reference_str)

    def build(self):
        cmake = CMake(self)
        cppelib_platform_test = self.dependencies["cppelib_platform_test"]
        cmake.configure(variables={"PLATFORM_OS": cppelib_platform_test.options.platform_os})
        cmake.build()

    def layout(self):
        cmake_layout(self)

    def test(self):
        if can_run(self):
            cmd = os.path.join(self.cpp.build.bindir, "PackageTest")
            self.run(cmd, env="conanrun")
