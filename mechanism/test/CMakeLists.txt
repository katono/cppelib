cmake_minimum_required(VERSION 2.8)
enable_testing()

project(mechanism_test)
enable_language(CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_BUILD_TYPE Debug)

include_directories(..)

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
