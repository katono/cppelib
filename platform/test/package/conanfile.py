from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.files import copy
import os
import re


class cppelib_platform_testRecipe(ConanFile):
    name = "cppelib_platform_test"
    version = "1.5.0"

    # Optional metadata
    license = "MIT License"
    author = "KATO Noriaki <katono123@gmail.com>"
    url = "https://github.com/katono/cppelib"
    description = (
        "CppELib is a portable C++ library for embedded software development. "
        "This package is packaged test code of 'platform' of CppELib. "
    )
    topics = ("embedded", "rtos")

    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False], "platform_os": ["POSIX", "WINDOWS", "STDCPP", "OTHER"]}
    default_options = {"shared": False, "fPIC": True, "platform_os": "STDCPP"}

    exports_sources = "CMakeLists.txt"

    def export_sources(self):
        copy(self, "LICENSE", src=os.path.join(self.recipe_folder, "..", "..", ".."), dst=self.export_sources_folder)
        copy(self, "PlatformOSWrapperTest/*", src=os.path.join(self.recipe_folder, ".."), dst=self.export_sources_folder)
        copy(self, "TestDoubleOSWrapperTest/*", src=os.path.join(self.recipe_folder, ".."), dst=self.export_sources_folder)

    def requirements(self):
        self.requires("cppelib_mechanism/1.5.0", transitive_headers=True, transitive_libs=True)
        self.requires("cppelib_platform/1.5.0", transitive_headers=True, transitive_libs=True)
        self.requires("cpputest/4.0", transitive_headers=True, transitive_libs=True)

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

    def package(self):
        copy(self, "LICENSE", src=self.source_folder, dst=os.path.join(self.package_folder, "licenses"))
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["cppelib_platform_test"]

    def package_id(self):
        value = self.conf.get("tools.build:cxxflags")
        self.info.conf.define("tools.build:cxxflags", value)
