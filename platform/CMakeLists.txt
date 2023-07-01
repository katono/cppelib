cmake_minimum_required(VERSION 3.15)
project(cppelib_platform CXX)

add_library(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE CPPELIB_PLATFORM_SRC_COMMON "StdCppOSWrapper/*.cpp")
if(MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE /W4)
	file(GLOB_RECURSE CPPELIB_PLATFORM_SRC "WindowsOSWrapper/*.cpp")
	set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/WindowsOSWrapper)
else()
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
	file(GLOB_RECURSE CPPELIB_PLATFORM_SRC "PosixOSWrapper/*.cpp")
	set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/PosixOSWrapper)
endif()
target_sources(${PROJECT_NAME} PRIVATE
	${CPPELIB_PLATFORM_SRC_COMMON} ${CPPELIB_PLATFORM_SRC})

find_package(cppelib_mechanism REQUIRED)
target_link_libraries(${PROJECT_NAME} cppelib_mechanism::cppelib_mechanism)

install(TARGETS ${PROJECT_NAME})
install(DIRECTORY
	${CMAKE_CURRENT_SOURCE_DIR}/StdCppOSWrapper
	${HEADER_DIR}
	DESTINATION "include" FILES_MATCHING PATTERN "*.h"
)
