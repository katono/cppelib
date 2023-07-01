cmake_minimum_required(VERSION 3.1)
enable_testing()

set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)

project(mechanism_cpp11_test)
message("PROJECT_NAME: ${PROJECT_NAME}")

set(CMAKE_CXX_STANDARD 11)

include(../BuildAlltests.cmake)

find_package(cppelib_mechanism REQUIRED)
target_link_libraries(${PROJECT_NAME} cppelib_mechanism::cppelib_mechanism)

find_package(cpputest REQUIRED)
target_link_libraries(${PROJECT_NAME} cpputest::cpputest)

add_custom_target(test_verbose COMMAND ${CMAKE_CTEST_COMMAND} --verbose)

add_test(NAME ${PROJECT_NAME}
	COMMAND ${PROJECT_NAME}
)
