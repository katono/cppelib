from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
import re


class runTestsRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False], "platform_os": ["POSIX", "WINDOWS", "STDCPP", "OTHER"]}
    default_options = {"shared": False, "fPIC": True, "platform_os": "STDCPP"}

    def requirements(self):
        self.requires("cpputest/4.0")

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def layout(self):
        compiler = self.settings.compiler
        compiler_version = compiler.version
        arch = self.settings.arch
        cppstd = compiler.cppstd
        platform = self.options.platform_os
        cxxflags = ""
        if self.conf.get("tools.build:cxxflags"):
            cxxflags = "".join(self.conf.get("tools.build:cxxflags"))
            cxxflags = re.sub("[^a-zA-Z0-9]", "_", cxxflags)
        cmake_layout(self, build_folder=f"build/{compiler}-{compiler_version}-{arch}-{cppstd}-{platform}-{cxxflags}")

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure(variables={"PLATFORM_OS": self.options.platform_os})
        cmake.build()
        cmake.test()

