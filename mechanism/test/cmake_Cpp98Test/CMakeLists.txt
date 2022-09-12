cmake_minimum_required(VERSION 3.1)
enable_testing()

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../..)
set(CPPUTEST_DIR ${ROOT_DIR}/tool/CppUTest/origin)
set(CPPUTEST_BUILD_DIR ${CPPUTEST_DIR}/cpputest_build)
set(MECHANISM_DIR ${ROOT_DIR}/mechanism)
set(PROJECT_NAME mechanism_test)

project(${PROJECT_NAME})
message("PROJECT_NAME: ${PROJECT_NAME}")

set(CMAKE_CXX_STANDARD 98)

include(../Config.cmake)
include(../BuildAlltests.cmake)
include(${CPPUTEST_DIR}/../BuildCppUTest.cmake)

target_link_libraries(alltests CppUTest CppUTestExt)

add_test(NAME ${PROJECT_NAME}
	COMMAND alltests
)
