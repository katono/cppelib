cmake_minimum_required(VERSION 3.15)
project(cppelib_platform_test)

message("CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")

if(MSVC)
	add_compile_options(/W4)
	add_compile_options(-DPLATFORM_OS_WINDOWS)
else()
	add_compile_options(-Wall -Wextra)
	add_compile_options(-DPLATFORM_OS_POSIX)
	set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
endif()

file(GLOB_RECURSE CPPELIB_PLATFORM_TEST_SRC
	"PlatformOSWrapperTest/*.cpp"
	"TestDoubleOSWrapperTest/*.cpp"
)
add_library(${PROJECT_NAME} ${CPPELIB_PLATFORM_TEST_SRC})

find_package(cppelib_mechanism REQUIRED)
target_link_libraries(${PROJECT_NAME} cppelib_mechanism::cppelib_mechanism)

find_package(cppelib_platform REQUIRED)
target_link_libraries(${PROJECT_NAME} cppelib_platform::cppelib_platform)

find_package(CppUTest REQUIRED)
target_link_libraries(${PROJECT_NAME} cpputest::cpputest)

install(TARGETS ${PROJECT_NAME})
install(DIRECTORY
	${CMAKE_CURRENT_SOURCE_DIR}/PlatformOSWrapperTest
	${CMAKE_CURRENT_SOURCE_DIR}/TestDoubleOSWrapperTest
	DESTINATION "include" FILES_MATCHING PATTERN "*.h"
)

