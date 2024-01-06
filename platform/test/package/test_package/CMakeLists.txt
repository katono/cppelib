cmake_minimum_required(VERSION 3.15)
project(PackageTest CXX)

message("PLATFORM_OS: ${PLATFORM_OS}")
add_compile_options("-DPLATFORM_OS_${PLATFORM_OS}")

add_executable(${PROJECT_NAME} src/main.cpp)

find_package(cppelib_platform_test REQUIRED)
target_link_libraries(${PROJECT_NAME} cppelib_platform_test::cppelib_platform_test)

if(MSVC)
	target_link_libraries(${PROJECT_NAME}
		-WHOLEARCHIVE:cppelib_platform_test.lib
	)
else()
	target_link_libraries(${PROJECT_NAME}
		-Wl,--whole-archive
		-lcppelib_platform_test
		-Wl,--no-whole-archive
	)
endif()
