cmake_minimum_required(VERSION 3.15)
project(mechanism_test)

message("CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")
message("CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
	add_compile_options(/W4)
else()
	add_compile_options(-Wall -Wextra)
	set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
endif()

set(SUT cppelib_mechanism)
set(SUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
file(GLOB_RECURSE CPPELIB_MECHANISM_SRC
	"${SUT_DIR}/Assertion/*.h"
	"${SUT_DIR}/Container/*.h"
	"${SUT_DIR}/OSWrapper/*.h"
	"${SUT_DIR}/OSWrapper/*.cpp"
)
add_library(${SUT} ${CPPELIB_MECHANISM_SRC})
target_include_directories(${SUT} PUBLIC ${SUT_DIR})

file(GLOB_RECURSE CPPELIB_MECHANISM_TEST_SRC
	"AssertionTest/*.cpp"
	"ContainerTest/*.cpp"
	"OSWrapperTest/*.cpp"
)

add_executable(${PROJECT_NAME}
	main.cpp
	${CPPELIB_MECHANISM_TEST_SRC}
)
target_include_directories(${PROJECT_NAME} PUBLIC ${SUT_DIR})
target_link_libraries(${PROJECT_NAME} ${SUT})

find_package(CppUTest REQUIRED)
target_link_libraries(${PROJECT_NAME} cpputest::cpputest)

list(APPEND CMAKE_CTEST_ARGUMENTS "--verbose")
enable_testing()
add_test(NAME ${PROJECT_NAME}
	COMMAND ${PROJECT_NAME}
)
