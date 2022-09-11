cmake_minimum_required(VERSION 3.1)
enable_testing()

set(CPPUTEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../tool/CppUTest/origin)

project(ContainerBenchmark)
enable_language(CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

include_directories(${CPPUTEST_DIR}/include)
include_directories(../..)

link_directories(${CPPUTEST_DIR}/src/CppUTest)
link_directories(${CPPUTEST_DIR}/src/CppUTestExt)

set(ContainerBenchmarkSrc
	ArrayBenchmark.cpp
	FixedDequeBenchmark.cpp
	FixedVectorBenchmark.cpp
)

add_executable(alltests
	../main.cpp
	${ContainerBenchmarkSrc}
)

target_link_libraries(alltests CppUTest CppUTestExt)

add_test(NAME ContainerBenchmark
	COMMAND alltests
)
