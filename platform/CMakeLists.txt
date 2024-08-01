cmake_minimum_required(VERSION 3.15)
project(cppelib_platform CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE CPPELIB_PLATFORM_SRC
	"TestDoubleOSWrapper/*.h"
	"TestDoubleOSWrapper/*.cpp"
)
set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/TestDoubleOSWrapper)
if(NOT "${CONAN_SETTINGS_OS}" STREQUAL "baremetal")
	file(GLOB_RECURSE CPPELIB_PLATFORM_SRC_STDCPP
		"StdCppOSWrapper/*.h"
		"StdCppOSWrapper/*.cpp"
	)
	list(APPEND CPPELIB_PLATFORM_SRC ${CPPELIB_PLATFORM_SRC_STDCPP})
	list(APPEND HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/StdCppOSWrapper)
	if(MSVC)
		target_compile_options(${PROJECT_NAME} PRIVATE /W4)
		file(GLOB_RECURSE CPPELIB_PLATFORM_SRC_WINDOWS
			"WindowsOSWrapper/*.h"
			"WindowsOSWrapper/*.cpp"
		)
		list(APPEND CPPELIB_PLATFORM_SRC ${CPPELIB_PLATFORM_SRC_WINDOWS})
		list(APPEND HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/WindowsOSWrapper)
	else()
		target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
		file(GLOB_RECURSE CPPELIB_PLATFORM_SRC_POSIX
			"PosixOSWrapper/*.h"
			"PosixOSWrapper/*.cpp"
		)
		list(APPEND CPPELIB_PLATFORM_SRC ${CPPELIB_PLATFORM_SRC_POSIX})
		list(APPEND HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/PosixOSWrapper)
		set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
	endif()
endif()
target_sources(${PROJECT_NAME} PRIVATE ${CPPELIB_PLATFORM_SRC})

find_package(cppelib_mechanism REQUIRED)
target_link_libraries(${PROJECT_NAME} cppelib_mechanism::cppelib_mechanism)

install(TARGETS ${PROJECT_NAME})
install(DIRECTORY
	${HEADER_DIR}
	DESTINATION "include" FILES_MATCHING PATTERN "*.h"
)
