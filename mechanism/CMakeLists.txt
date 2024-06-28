cmake_minimum_required(VERSION 3.15)
project(cppelib_mechanism CXX)

file(GLOB_RECURSE CPPELIB_MECHANISM_SRC
	"Assertion/*.h"
	"Container/*.h"
	"OSWrapper/*.h"
	"OSWrapper/*.cpp"
)

add_library(${PROJECT_NAME} ${CPPELIB_MECHANISM_SRC})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
	set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
endif()

install(TARGETS ${PROJECT_NAME})
install(DIRECTORY
	${CMAKE_CURRENT_SOURCE_DIR}/Assertion
	${CMAKE_CURRENT_SOURCE_DIR}/Container
	${CMAKE_CURRENT_SOURCE_DIR}/OSWrapper
	DESTINATION "include" FILES_MATCHING PATTERN "*.h"
)
