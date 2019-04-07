cmake_minimum_required(VERSION 2.8)
enable_testing()

set(CPPUTEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../tool/CppUTest)

project(platform_test)

enable_language(CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -DPLATFORM_OS_STDCPP")
set(CMAKE_BUILD_TYPE Release)

include_directories(${CPPUTEST_DIR}/include)
include_directories(../../mechanism)
include_directories(..)

link_directories(${CPPUTEST_DIR}/src/CppUTest)
link_directories(${CPPUTEST_DIR}/src/CppUTestExt)


set(OSWrapperSrc
	../../mechanism/OSWrapper/EventFlag.cpp
	../../mechanism/OSWrapper/FixedMemoryPool.cpp
	../../mechanism/OSWrapper/Mutex.cpp
	../../mechanism/OSWrapper/PeriodicTimer.cpp
	../../mechanism/OSWrapper/Thread.cpp
	../../mechanism/OSWrapper/Timeout.cpp
	../../mechanism/OSWrapper/VariableMemoryPool.cpp
)

set(StdCppOSWrapperSrc
	../StdCppOSWrapper/StdCppEventFlagFactory.cpp
	../StdCppOSWrapper/StdCppFixedMemoryPoolFactory.cpp
	../StdCppOSWrapper/StdCppMutexFactory.cpp
	../StdCppOSWrapper/StdCppThreadFactory.cpp
	../StdCppOSWrapper/StdCppVariableMemoryPoolFactory.cpp
	../StdCppOSWrapper/StdCppOSWrapper.cpp
)

set(PlatformOSWrapperTestSrc
	PlatformOSWrapperTest/PlatformEventFlagTest.cpp
	PlatformOSWrapperTest/PlatformFixedMemoryPoolTest.cpp
	PlatformOSWrapperTest/PlatformMessageQueueTest.cpp
	PlatformOSWrapperTest/PlatformMutexTest.cpp
	PlatformOSWrapperTest/PlatformThreadTest.cpp
	PlatformOSWrapperTest/PlatformVariableMemoryPoolTest.cpp
	PlatformOSWrapperTest/PlatformOSWrapperInitTest.cpp
)

add_executable(alltests
	main.cpp
	${OSWrapperSrc}
	${StdCppOSWrapperSrc}
	${PlatformOSWrapperTestSrc}
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

find_package(Threads REQUIRED)

target_link_libraries(alltests CppUTest CppUTestExt ${CMAKE_THREAD_LIBS_INIT})

add_test(NAME platform_test
	COMMAND alltests
)
