cmake_minimum_required(VERSION 3.15)
project(platform_test)

message("CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")
message("CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message("PLATFORM_OS: ${PLATFORM_OS}")

add_compile_options("-DPLATFORM_OS_${PLATFORM_OS}")

if(MSVC)
	add_compile_options(/W4)
else()
	add_compile_options(-Wall -Wextra)
	set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
endif()

set(SUT_MECHANISM cppelib_mechanism)
set(SUT_MECHANISM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../mechanism)
file(GLOB_RECURSE CPPELIB_MECHANISM_SRC
	"${SUT_MECHANISM_DIR}/Assertion/*.h"
	"${SUT_MECHANISM_DIR}/Container/*.h"
	"${SUT_MECHANISM_DIR}/OSWrapper/*.h"
	"${SUT_MECHANISM_DIR}/OSWrapper/*.cpp"
)
add_library(${SUT_MECHANISM} ${CPPELIB_MECHANISM_SRC})
target_include_directories(${SUT_MECHANISM} PUBLIC ${SUT_MECHANISM_DIR})

set(SUT_PLATFORM cppelib_platform)
set(SUT_PLATFORM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
file(GLOB_RECURSE CPPELIB_PLATFORM_SRC_COMMON
	"${SUT_PLATFORM_DIR}/StdCppOSWrapper/*.h"
	"${SUT_PLATFORM_DIR}/StdCppOSWrapper/*.cpp"
	"${SUT_PLATFORM_DIR}/TestDoubleOSWrapper/*.h"
	"${SUT_PLATFORM_DIR}/TestDoubleOSWrapper/*.cpp"
)
if(MSVC)
	file(GLOB_RECURSE CPPELIB_PLATFORM_SRC
		"${SUT_PLATFORM_DIR}/WindowsOSWrapper/*.h"
		"${SUT_PLATFORM_DIR}/WindowsOSWrapper/*.cpp"
	)
else()
	file(GLOB_RECURSE CPPELIB_PLATFORM_SRC
		"${SUT_PLATFORM_DIR}/PosixOSWrapper/*.h"
		"${SUT_PLATFORM_DIR}/PosixOSWrapper/*.cpp"
	)
endif()
add_library(${SUT_PLATFORM} ${CPPELIB_PLATFORM_SRC_COMMON} ${CPPELIB_PLATFORM_SRC})
target_include_directories(${SUT_PLATFORM} PUBLIC ${SUT_MECHANISM_DIR} ${SUT_PLATFORM_DIR})

file(GLOB_RECURSE CPPELIB_PLATFORM_TEST_SRC
	"PlatformOSWrapperTest/*.h"
	"PlatformOSWrapperTest/*.cpp"
	"TestDoubleOSWrapperTest/*.cpp"
)

add_executable(${PROJECT_NAME}
	main.cpp
	${CPPELIB_PLATFORM_TEST_SRC}
)
target_include_directories(${PROJECT_NAME} PUBLIC ${SUT_MECHANISM_DIR} ${SUT_PLATFORM_DIR})
target_link_libraries(${PROJECT_NAME} ${SUT_MECHANISM} ${SUT_PLATFORM})

find_package(CppUTest REQUIRED)
target_link_libraries(${PROJECT_NAME} cpputest::cpputest)

list(APPEND CMAKE_CTEST_ARGUMENTS "--verbose")
enable_testing()
add_test(NAME ${PROJECT_NAME}
	COMMAND ${PROJECT_NAME}
)
