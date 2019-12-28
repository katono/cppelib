cmake_minimum_required(VERSION 2.8)
enable_testing()

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../..)
set(CPPUTEST_DIR ${ROOT_DIR}/tool/CppUTest/origin)
set(MECHANISM_DIR ${ROOT_DIR}/mechanism)
set(PLATFORM_DIR ${ROOT_DIR}/platform)

project(platform_test)

enable_language(CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -DPLATFORM_OS_STDCPP")
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()
message("CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message("CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message("CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")

include_directories(${CPPUTEST_DIR}/include)
include_directories(${MECHANISM_DIR})
include_directories(${PLATFORM_DIR})

link_directories(${CPPUTEST_DIR}/src/CppUTest)
link_directories(${CPPUTEST_DIR}/src/CppUTestExt)


set(OSWrapperSrc
	${MECHANISM_DIR}/OSWrapper/EventFlag.cpp
	${MECHANISM_DIR}/OSWrapper/FixedMemoryPool.cpp
	${MECHANISM_DIR}/OSWrapper/Mutex.cpp
	${MECHANISM_DIR}/OSWrapper/OneShotTimer.cpp
	${MECHANISM_DIR}/OSWrapper/PeriodicTimer.cpp
	${MECHANISM_DIR}/OSWrapper/Thread.cpp
	${MECHANISM_DIR}/OSWrapper/Timeout.cpp
	${MECHANISM_DIR}/OSWrapper/VariableMemoryPool.cpp
)

set(StdCppOSWrapperSrc
	${PLATFORM_DIR}/StdCppOSWrapper/StdCppEventFlagFactory.cpp
	${PLATFORM_DIR}/StdCppOSWrapper/StdCppFixedMemoryPoolFactory.cpp
	${PLATFORM_DIR}/StdCppOSWrapper/StdCppMutexFactory.cpp
	${PLATFORM_DIR}/StdCppOSWrapper/StdCppOneShotTimerFactory.cpp
	${PLATFORM_DIR}/StdCppOSWrapper/StdCppPeriodicTimerFactory.cpp
	${PLATFORM_DIR}/StdCppOSWrapper/StdCppThreadFactory.cpp
	${PLATFORM_DIR}/StdCppOSWrapper/StdCppVariableMemoryPoolFactory.cpp
	${PLATFORM_DIR}/StdCppOSWrapper/StdCppOSWrapper.cpp
)

set(PlatformOSWrapperTestSrc
	${PLATFORM_DIR}/test/PlatformOSWrapperTest/PlatformEventFlagTest.cpp
	${PLATFORM_DIR}/test/PlatformOSWrapperTest/PlatformFixedMemoryPoolTest.cpp
	${PLATFORM_DIR}/test/PlatformOSWrapperTest/PlatformMessageQueueTest.cpp
	${PLATFORM_DIR}/test/PlatformOSWrapperTest/PlatformMutexTest.cpp
	${PLATFORM_DIR}/test/PlatformOSWrapperTest/PlatformOneShotTimerTest.cpp
	${PLATFORM_DIR}/test/PlatformOSWrapperTest/PlatformPeriodicTimerTest.cpp
	${PLATFORM_DIR}/test/PlatformOSWrapperTest/PlatformThreadTest.cpp
	${PLATFORM_DIR}/test/PlatformOSWrapperTest/PlatformVariableMemoryPoolTest.cpp
	${PLATFORM_DIR}/test/PlatformOSWrapperTest/PlatformOSWrapperInitTest.cpp
)

add_executable(alltests
	${PLATFORM_DIR}/test/main.cpp
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
