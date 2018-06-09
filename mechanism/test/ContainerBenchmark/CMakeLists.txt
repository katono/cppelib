project(ContainerBenchmark)
enable_language(CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

include_directories(../..)

set(ContainerBenchmarkSrc
	ArrayBenchmark.cpp
	FixedDequeBenchmark.cpp
	FixedVectorBenchmark.cpp
)

add_executable(ContainerBenchmark
	../main.cpp
	${ContainerBenchmarkSrc}
)

target_link_libraries(ContainerBenchmark CppUTest CppUTestExt)
