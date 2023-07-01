cmake_minimum_required(VERSION 3.15)
project(assertion_no_exceptions_test)

add_compile_options("-DCPPELIB_NO_EXCEPTIONS")
add_compile_options("-fno-exceptions")
add_compile_options("-fno-rtti")

file(GLOB CPPELIB_MECHANISM_TEST_SRC "*.cpp")

add_executable(${PROJECT_NAME}
	${CPPELIB_MECHANISM_TEST_SRC}
)
set(SUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
target_include_directories(${PROJECT_NAME} PUBLIC ${SUT_DIR})

enable_testing()
add_test(NAME ${PROJECT_NAME}
	COMMAND ${PROJECT_NAME}
)
