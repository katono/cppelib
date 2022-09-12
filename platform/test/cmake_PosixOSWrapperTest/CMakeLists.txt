cmake_minimum_required(VERSION 3.1)
enable_testing()

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../..)
set(CPPUTEST_DIR ${ROOT_DIR}/tool/CppUTest/origin)
set(CPPUTEST_BUILD_DIR ${CPPUTEST_DIR}/cpputest_build)
set(MECHANISM_DIR ${ROOT_DIR}/mechanism)
set(PLATFORM_DIR ${ROOT_DIR}/platform)
set(PROJECT_NAME platform_test)

project(${PROJECT_NAME})
message("PROJECT_NAME: ${PROJECT_NAME}")

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPLATFORM_OS_POSIX")
include(${MECHANISM_DIR}/test/Config.cmake)
include(${CPPUTEST_DIR}/../BuildCppUTest.cmake)

include_directories(${CPPUTEST_DIR}/include)
include_directories(${MECHANISM_DIR})
include_directories(${PLATFORM_DIR})

link_directories(${CPPUTEST_BUILD_DIR}/src/CppUTest)
link_directories(${CPPUTEST_BUILD_DIR}/src/CppUTestExt)


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

set(StdCppOSWrapperSrc
	${PLATFORM_DIR}/StdCppOSWrapper/StdCppEventFlagFactory.cpp
	${PLATFORM_DIR}/StdCppOSWrapper/StdCppFixedMemoryPoolFactory.cpp
	${PLATFORM_DIR}/StdCppOSWrapper/StdCppMutexFactory.cpp
	${PLATFORM_DIR}/StdCppOSWrapper/StdCppOneShotTimerFactory.cpp
	${PLATFORM_DIR}/StdCppOSWrapper/StdCppPeriodicTimerFactory.cpp
	${PLATFORM_DIR}/StdCppOSWrapper/StdCppThreadFactory.cpp
	${PLATFORM_DIR}/StdCppOSWrapper/StdCppVariableMemoryPoolFactory.cpp
)

set(PosixOSWrapperSrc
	${PLATFORM_DIR}/PosixOSWrapper/PosixThreadFactory.cpp
	${PLATFORM_DIR}/PosixOSWrapper/PosixOSWrapper.cpp
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
	${PLATFORM_DIR}/test/PlatformOSWrapperTest/PlatformThreadPoolTest.cpp
)

add_executable(alltests
	${PLATFORM_DIR}/test/main.cpp
	${OSWrapperSrc}
	${StdCppOSWrapperSrc}
	${PosixOSWrapperSrc}
	${PlatformOSWrapperTestSrc}
)

find_package(Threads REQUIRED)

target_link_libraries(alltests CppUTest CppUTestExt ${CMAKE_THREAD_LIBS_INIT})

add_test(NAME ${PROJECT_NAME}
	COMMAND alltests
)
