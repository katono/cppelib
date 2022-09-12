cmake_minimum_required(VERSION 3.1)
enable_testing()

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../..)
set(MECHANISM_DIR ${ROOT_DIR}/mechanism)
set(PROJECT_NAME assertion_no_exceptions_test)

project(${PROJECT_NAME})
message("PROJECT_NAME: ${PROJECT_NAME}")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCPPELIB_NO_EXCEPTIONS")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
include(../Config.cmake)

include_directories(${MECHANISM_DIR})

set(AssertionNoExceptionsTestSrc
	${MECHANISM_DIR}/test/AssertionNoExceptionsTest/AssertionNoExceptionsTest.cpp
)

add_executable(alltests
	${AssertionNoExceptionsTestSrc}
)

add_test(NAME ${PROJECT_NAME}
	COMMAND alltests
)
