cmake_minimum_required(VERSION 2.8)
enable_testing()

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(CPPUTEST_DIR ${ROOT_DIR}/tool/CppUTest/origin)
set(MECHANISM_DIR ${ROOT_DIR}/mechanism)

project(mechanism_test)

enable_language(CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()
message("CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message("CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message("CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")

include_directories(${CPPUTEST_DIR}/include)
include_directories(${MECHANISM_DIR})

link_directories(${CPPUTEST_DIR}/src/CppUTest)
link_directories(${CPPUTEST_DIR}/src/CppUTestExt)

set(AssertionTestSrc
	${MECHANISM_DIR}/test/AssertionTest/AssertionTest.cpp
)

set(ContainerTestSrc
	${MECHANISM_DIR}/test/ContainerTest/ArrayTest.cpp
	${MECHANISM_DIR}/test/ContainerTest/BitPatternTest.cpp
	${MECHANISM_DIR}/test/ContainerTest/FixedDequeTest.cpp
	${MECHANISM_DIR}/test/ContainerTest/FixedVectorTest.cpp
	${MECHANISM_DIR}/test/ContainerTest/IntrusiveListTest.cpp
	${MECHANISM_DIR}/test/ContainerTest/PreallocatedDequeTest.cpp
	${MECHANISM_DIR}/test/ContainerTest/PreallocatedVectorTest.cpp
)

set(OSWrapperSrc
	${MECHANISM_DIR}/OSWrapper/EventFlag.cpp
	${MECHANISM_DIR}/OSWrapper/FixedMemoryPool.cpp
	${MECHANISM_DIR}/OSWrapper/Mutex.cpp
	${MECHANISM_DIR}/OSWrapper/OneShotTimer.cpp
	${MECHANISM_DIR}/OSWrapper/PeriodicTimer.cpp
	${MECHANISM_DIR}/OSWrapper/Thread.cpp
	${MECHANISM_DIR}/OSWrapper/Timeout.cpp
	${MECHANISM_DIR}/OSWrapper/VariableMemoryPool.cpp
	${MECHANISM_DIR}/OSWrapper/ThreadPool.cpp
)

set(OSWrapperTestSrc
	${MECHANISM_DIR}/test/OSWrapperTest/EventFlagTest.cpp
	${MECHANISM_DIR}/test/OSWrapperTest/FixedMemoryPoolTest.cpp
	${MECHANISM_DIR}/test/OSWrapperTest/MutexTest.cpp
	${MECHANISM_DIR}/test/OSWrapperTest/OneShotTimerTest.cpp
	${MECHANISM_DIR}/test/OSWrapperTest/PeriodicTimerTest.cpp
	${MECHANISM_DIR}/test/OSWrapperTest/ThreadTest.cpp
	${MECHANISM_DIR}/test/OSWrapperTest/VariableMemoryPoolTest.cpp
	${MECHANISM_DIR}/test/OSWrapperTest/MessageQueueTest.cpp
	${MECHANISM_DIR}/test/OSWrapperTest/ThreadPoolTest.cpp
)

add_executable(alltests
	${MECHANISM_DIR}/test/main.cpp
	${AssertionTestSrc}
	${ContainerTestSrc}
	${OSWrapperSrc}
	${OSWrapperTestSrc}
)

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


target_link_libraries(alltests CppUTest CppUTestExt)

add_test(NAME mechanism_test
	COMMAND alltests
)
