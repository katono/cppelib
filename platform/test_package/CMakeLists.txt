cmake_minimum_required(VERSION 3.15)
project(PackageTest CXX)

add_executable(${PROJECT_NAME} src/example.cpp)
if(NOT "${PLATFORM_OS}" STREQUAL "baremetal")
	if(MSVC)
		target_compile_options(${PROJECT_NAME} PRIVATE -DPLATFORM_OS_WINDOWS)
	else()
		target_compile_options(${PROJECT_NAME} PRIVATE -DPLATFORM_OS_POSIX)
	endif()
endif()

find_package(cppelib_platform REQUIRED)
target_link_libraries(${PROJECT_NAME} cppelib_platform::cppelib_platform)

find_package(cppelib_mechanism REQUIRED)
target_link_libraries(${PROJECT_NAME} cppelib_mechanism::cppelib_mechanism)
