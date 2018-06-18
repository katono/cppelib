cmake_minimum_required(VERSION 2.8)
enable_testing()

set(CPPUTEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../tool/CppUTest)

if(NOT EXISTS ${CPPUTEST_DIR}/src)
	message("Start: Get CppUTest")
	execute_process(
		COMMAND git submodule update --init
	)
	message("Done: Get CppUTest")
endif()

message("Start: Build CppUTest")
execute_process(
	COMMAND ${CMAKE_COMMAND} .
	WORKING_DIRECTORY ${CPPUTEST_DIR}
)
execute_process(
	COMMAND ${CMAKE_MAKE_PROGRAM} CppUTest CppUTestExt
	WORKING_DIRECTORY ${CPPUTEST_DIR}
)
message("Done: Build CppUTest")


project(mechanism_test)

enable_language(CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_BUILD_TYPE Debug)

include_directories(${CPPUTEST_DIR}/include)
include_directories(..)

link_directories(${CPPUTEST_DIR}/src/CppUTest)
link_directories(${CPPUTEST_DIR}/src/CppUTestExt)

set(AssertionTestSrc
	AssertionTest/AssertionTest.cpp
)

set(ContainerTestSrc
	ContainerTest/ArrayTest.cpp
	ContainerTest/BitPatternTest.cpp
	ContainerTest/FixedDequeTest.cpp
	ContainerTest/FixedVectorTest.cpp
	ContainerTest/IntrusiveListTest.cpp
)

set(OSWrapperSrc
	../OSWrapper/EventFlag.cpp
	../OSWrapper/FixedMemoryPool.cpp
	../OSWrapper/Mutex.cpp
	../OSWrapper/Thread.cpp
	../OSWrapper/Timeout.cpp
	../OSWrapper/VariableMemoryPool.cpp
)

set(OSWrapperTestSrc
	OSWrapperTest/EventFlagTest.cpp
	OSWrapperTest/FixedMemoryPoolTest.cpp
	OSWrapperTest/MutexTest.cpp
	OSWrapperTest/ThreadTest.cpp
	OSWrapperTest/VariableMemoryPoolTest.cpp
)

add_executable(alltests
	main.cpp
	${AssertionTestSrc}
	${ContainerTestSrc}
	${OSWrapperSrc}
	${OSWrapperTestSrc}
)

target_link_libraries(alltests CppUTest CppUTestExt)

add_test(NAME mechanism_test
	COMMAND alltests
)
