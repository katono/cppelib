cmake_minimum_required(VERSION 3.15)
project(cppelib_mechanism_test)

message("CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")
message("CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

if(MSVC)
	add_compile_options(/W4)
else()
	add_compile_options(-Wall -Wextra)
	set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
endif()

file(GLOB_RECURSE CPPELIB_MECHANISM_TEST_SRC
	"AssertionTest/*.cpp"
	"ContainerTest/*.cpp"
	"OSWrapperTest/*.cpp"
)
add_library(${PROJECT_NAME} ${CPPELIB_MECHANISM_TEST_SRC})

find_package(cppelib_mechanism REQUIRED)
target_link_libraries(${PROJECT_NAME} cppelib_mechanism::cppelib_mechanism)

find_package(CppUTest REQUIRED)
target_link_libraries(${PROJECT_NAME} cpputest::cpputest)

install(TARGETS ${PROJECT_NAME})
install(DIRECTORY
	${CMAKE_CURRENT_SOURCE_DIR}/AssertionTest
	${CMAKE_CURRENT_SOURCE_DIR}/ContainerTest
	${CMAKE_CURRENT_SOURCE_DIR}/OSWrapperTest
	DESTINATION "include" FILES_MATCHING PATTERN "*.h"
)

