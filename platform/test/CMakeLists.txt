cmake_minimum_required(VERSION 3.15)
project(platform_test)

message("CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")

if(MSVC)
	add_compile_options(/W4)
	add_compile_options(-DPLATFORM_OS_WINDOWS)
else()
	add_compile_options(-Wall -Wextra)
	add_compile_options(-DPLATFORM_OS_POSIX)
endif()

file(GLOB_RECURSE CPPELIB_PLATFORM_TEST_SRC
	"PlatformOSWrapperTest/*.cpp"
)

add_executable(${PROJECT_NAME}
	main.cpp
	${CPPELIB_PLATFORM_TEST_SRC}
)

find_package(cppelib_mechanism REQUIRED)
target_link_libraries(${PROJECT_NAME} cppelib_mechanism::cppelib_mechanism)

find_package(cppelib_platform REQUIRED)
target_link_libraries(${PROJECT_NAME} cppelib_platform::cppelib_platform)

find_package(cpputest REQUIRED)
target_link_libraries(${PROJECT_NAME} cpputest::cpputest)

enable_testing()
add_test(NAME ${PROJECT_NAME}
	COMMAND ${PROJECT_NAME}
)

add_subdirectory(OSWrapperNoExceptionsTest)
