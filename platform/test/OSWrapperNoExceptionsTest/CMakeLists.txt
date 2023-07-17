cmake_minimum_required(VERSION 3.15)
project(oswrapper_no_exceptions_test)

add_compile_options(-DCPPELIB_NO_EXCEPTIONS)
if(MSVC)
	add_compile_options(/W4)
	add_compile_options(-DPLATFORM_OS_WINDOWS)
else()
	add_compile_options(-Wall -Wextra)
	add_compile_options(-DPLATFORM_OS_POSIX)
	add_compile_options(-fno-exceptions)
	add_compile_options(-fno-rtti)
endif()

set(SUT_MECHANISM cppelib_mechanism_no_exceptions)
set(SUT_MECHANISM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../mechanism)
file(GLOB_RECURSE CPPELIB_MECHANISM_SRC "${SUT_MECHANISM_DIR}/OSWrapper/*.cpp")
add_library(${SUT_MECHANISM} ${CPPELIB_MECHANISM_SRC})
target_include_directories(${SUT_MECHANISM} PUBLIC ${SUT_MECHANISM_DIR})

set(SUT_PLATFORM cppelib_platform_no_exceptions)
set(SUT_PLATFORM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
file(GLOB_RECURSE CPPELIB_PLATFORM_SRC_COMMON "${SUT_PLATFORM_DIR}/StdCppOSWrapper/*.cpp")
if(MSVC)
	file(GLOB_RECURSE CPPELIB_PLATFORM_SRC "${SUT_PLATFORM_DIR}/WindowsOSWrapper/*.cpp")
else()
	file(GLOB_RECURSE CPPELIB_PLATFORM_SRC "${SUT_PLATFORM_DIR}/PosixOSWrapper/*.cpp")
endif()
add_library(${SUT_PLATFORM} ${CPPELIB_PLATFORM_SRC_COMMON} ${CPPELIB_PLATFORM_SRC})
target_include_directories(${SUT_PLATFORM} PUBLIC ${SUT_MECHANISM_DIR} ${SUT_PLATFORM_DIR})

file(GLOB_RECURSE CPPELIB_PLATFORM_TEST_SRC
	"../PlatformOSWrapperTest/*.cpp"
)

add_executable(${PROJECT_NAME}
	main.cpp
	${CPPELIB_PLATFORM_TEST_SRC}
)
target_include_directories(${PROJECT_NAME} PUBLIC ${SUT_MECHANISM_DIR} ${SUT_PLATFORM_DIR})
target_link_libraries(${PROJECT_NAME} ${SUT_MECHANISM} ${SUT_PLATFORM})

find_package(CppUTest REQUIRED)
target_link_libraries(${PROJECT_NAME} cpputest::cpputest)

enable_testing()
add_test(NAME ${PROJECT_NAME}
	COMMAND ${PROJECT_NAME}
)
