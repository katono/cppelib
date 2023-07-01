cmake_minimum_required(VERSION 3.15)
project(PackageTest CXX)

add_executable(example src/example.cpp)

find_package(cppelib_platform REQUIRED)
target_link_libraries(example cppelib_platform::cppelib_platform)

find_package(cppelib_mechanism REQUIRED)
target_link_libraries(example cppelib_mechanism::cppelib_mechanism)
