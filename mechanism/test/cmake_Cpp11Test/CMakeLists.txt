cmake_minimum_required(VERSION 3.15)

message("CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")

set(SUT cppelib_mechanism)
set(SUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
file(GLOB_RECURSE CPPELIB_MECHANISM_SRC "${SUT_DIR}/OSWrapper/*.cpp")
add_library(${SUT} ${CPPELIB_MECHANISM_SRC})
target_include_directories(${SUT} PUBLIC ${SUT_DIR})

project(mechanism_cpp11_test)

set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
file(GLOB_RECURSE CPPELIB_MECHANISM_TEST_SRC
	"${TEST_DIR}/AssertionTest/*.cpp"
	"${TEST_DIR}/ContainerTest/*.cpp"
	"${TEST_DIR}/OSWrapperTest/*.cpp"
)

add_executable(${PROJECT_NAME}
	${TEST_DIR}/main.cpp
	${CPPELIB_MECHANISM_TEST_SRC}
)
target_include_directories(${PROJECT_NAME} PUBLIC ${SUT_DIR})
target_link_libraries(${PROJECT_NAME} ${SUT})

find_package(cpputest REQUIRED)
target_link_libraries(${PROJECT_NAME} cpputest::cpputest)

enable_testing()
add_test(NAME ${PROJECT_NAME}
	COMMAND ${PROJECT_NAME}
)
