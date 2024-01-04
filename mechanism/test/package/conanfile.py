from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.files import copy
import os
import re


class cppelib_mechanism_testRecipe(ConanFile):
    name = "cppelib_mechanism_test"
    version = "1.5.0"

    # Optional metadata
    license = "MIT License"
    author = "KATO Noriaki <katono123@gmail.com>"
    url = "https://github.com/katono/cppelib"
    description = (
        "CppELib is a portable C++ library for embedded software development. "
        "This package is packaged test code of 'mechanism' of CppELib. "
    )
    topics = ("embedded", "rtos")

    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}

    exports_sources = "CMakeLists.txt"

    def export_sources(self):
        copy(self, "LICENSE", src=os.path.join(self.recipe_folder, "..", "..", ".."), dst=self.export_sources_folder)
        copy(self, "AssertionTest/*", src=os.path.join(self.recipe_folder, ".."), dst=self.export_sources_folder)
        copy(self, "ContainerTest/*", src=os.path.join(self.recipe_folder, ".."), dst=self.export_sources_folder)
        copy(self, "OSWrapperTest/*", src=os.path.join(self.recipe_folder, ".."), dst=self.export_sources_folder)

    def requirements(self):
        self.requires("cppelib_mechanism/1.5.0")
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
        cxxflags = ""
        if self.conf.get("tools.build:cxxflags"):
            cxxflags = "".join(self.conf.get("tools.build:cxxflags"))
            cxxflags = re.sub("[^a-zA-Z0-9]", "_", cxxflags)
        cmake_layout(self, build_folder=f"build/{compiler}-{compiler_version}-{arch}-{cppstd}-{cxxflags}")

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        copy(self, "LICENSE", src=self.source_folder, dst=os.path.join(self.package_folder, "licenses"))
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["cppelib_mechanism_test"]

    def package_id(self):
        value = self.conf.get("tools.build:cxxflags")
        self.info.conf.define("tools.build:cxxflags", value)
