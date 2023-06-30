cmake_minimum_required(VERSION 3.15)
project(cppelib_mechanism CXX)

set(CPPELIB_MECHANISM_SRC
	OSWrapper/EventFlag.cpp
	OSWrapper/FixedMemoryPool.cpp
	OSWrapper/Mutex.cpp
	OSWrapper/OneShotTimer.cpp
	OSWrapper/PeriodicTimer.cpp
	OSWrapper/Thread.cpp
	OSWrapper/ThreadPool.cpp
	OSWrapper/Timeout.cpp
	OSWrapper/VariableMemoryPool.cpp
)

add_library(cppelib_mechanism ${CPPELIB_MECHANISM_SRC})
target_include_directories(cppelib_mechanism PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS cppelib_mechanism)
install(DIRECTORY
	${CMAKE_CURRENT_SOURCE_DIR}/Assertion
	${CMAKE_CURRENT_SOURCE_DIR}/Container
	${CMAKE_CURRENT_SOURCE_DIR}/OSWrapper
	DESTINATION "include" FILES_MATCHING PATTERN "*.h"
)
