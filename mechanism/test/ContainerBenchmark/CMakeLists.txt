cmake_minimum_required(VERSION 3.15)
project(container_benchmark)

if(MSVC)
	add_compile_options(/W4)
else()
	add_compile_options(-Wall -Wextra)
endif()

file(GLOB CPPELIB_MECHANISM_TEST_SRC "*.cpp")

add_executable(${PROJECT_NAME}
	${CPPELIB_MECHANISM_TEST_SRC}
	../main.cpp
)
set(SUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
target_include_directories(${PROJECT_NAME} PUBLIC ${SUT_DIR})

find_package(cpputest REQUIRED)
target_link_libraries(${PROJECT_NAME} cpputest::cpputest)

enable_testing()
add_test(NAME ${PROJECT_NAME}
	COMMAND ${PROJECT_NAME}
)
