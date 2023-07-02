cmake_minimum_required(VERSION 3.15)
project(PackageTest CXX)

if(MSVC)
	add_compile_options(-DPLATFORM_OS_WINDOWS)
else()
	add_compile_options(-DPLATFORM_OS_POSIX)
endif()

add_executable(example src/example.cpp)

find_package(cppelib_platform REQUIRED)
target_link_libraries(example cppelib_platform::cppelib_platform)

find_package(cppelib_mechanism REQUIRED)
target_link_libraries(example cppelib_mechanism::cppelib_mechanism)
