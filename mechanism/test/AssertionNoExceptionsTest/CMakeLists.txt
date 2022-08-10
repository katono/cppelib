cmake_minimum_required(VERSION 2.8)
enable_testing()

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../..)
set(MECHANISM_DIR ${ROOT_DIR}/mechanism)

project(assertion_no_exceptions_test)

enable_language(CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCPPELIB_NO_EXCEPTIONS")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()
message("CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message("CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message("CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")

include_directories(${MECHANISM_DIR})


set(AssertionNoExceptionsTestSrc
	${MECHANISM_DIR}/test/AssertionNoExceptionsTest/AssertionNoExceptionsTest.cpp
)

add_executable(alltests
	${AssertionNoExceptionsTestSrc}
)

add_test(NAME assertion_no_exceptions_test
	COMMAND alltests
)
