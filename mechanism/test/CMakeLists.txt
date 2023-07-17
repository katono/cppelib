cmake_minimum_required(VERSION 3.15)
project(mechanism_test)

message("CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")

if(MSVC)
	add_compile_options(/W4)
else()
	add_compile_options(-Wall -Wextra)
	set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
endif()

set(SUT cppelib_mechanism)
set(SUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
file(GLOB_RECURSE CPPELIB_MECHANISM_SRC "${SUT_DIR}/OSWrapper/*.cpp")
add_library(${SUT} ${CPPELIB_MECHANISM_SRC})
target_include_directories(${SUT} PUBLIC ${SUT_DIR})

file(GLOB_RECURSE CPPELIB_MECHANISM_TEST_SRC
	"AssertionTest/*.cpp"
	"ContainerTest/*.cpp"
	"OSWrapperTest/*.cpp"
)

add_executable(${PROJECT_NAME}
	main.cpp
	${CPPELIB_MECHANISM_TEST_SRC}
)
target_include_directories(${PROJECT_NAME} PUBLIC ${SUT_DIR})
target_link_libraries(${PROJECT_NAME} ${SUT})

find_package(CppUTest REQUIRED)
target_link_libraries(${PROJECT_NAME} cpputest::cpputest)

list(APPEND CMAKE_CTEST_ARGUMENTS "--verbose")
enable_testing()
add_test(NAME ${PROJECT_NAME}
	COMMAND ${PROJECT_NAME}
)

if(NOT ${CMAKE_CXX_STANDARD} EQUAL 98)
	add_subdirectory(AssertionNoExceptionsTest)
	add_subdirectory(ContainerNoExceptionsTest)
endif()
