cmake_minimum_required(VERSION 3.1)
enable_testing()

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../..)
set(CPPUTEST_DIR ${ROOT_DIR}/tool/CppUTest/origin)
set(CPPUTEST_BUILD_DIR ${CPPUTEST_DIR}/cpputest_build)
set(MECHANISM_DIR ${ROOT_DIR}/mechanism)
set(PROJECT_NAME container_benchmark)

project(${PROJECT_NAME})
message("PROJECT_NAME: ${PROJECT_NAME}")

set(CMAKE_CXX_STANDARD 11)

include(../Config.cmake)
include(${CPPUTEST_DIR}/../BuildCppUTest.cmake)

include_directories(${CPPUTEST_DIR}/include)
include_directories(${MECHANISM_DIR})

link_directories(${CPPUTEST_BUILD_DIR}/src/CppUTest)
link_directories(${CPPUTEST_BUILD_DIR}/src/CppUTestExt)

set(ContainerBenchmarkSrc
	ArrayBenchmark.cpp
	FixedDequeBenchmark.cpp
	FixedVectorBenchmark.cpp
)

add_executable(alltests
	${MECHANISM_DIR}/test/main.cpp
	${ContainerBenchmarkSrc}
)

target_link_libraries(alltests CppUTest CppUTestExt)

add_test(NAME ${PROJECT_NAME}
	COMMAND alltests
)

