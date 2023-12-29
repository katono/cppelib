cmake_minimum_required(VERSION 3.15)
project(PackageTest CXX)

add_executable(${PROJECT_NAME} src/main.cpp)

find_package(cppelib_mechanism_test REQUIRED)
target_link_libraries(${PROJECT_NAME} cppelib_mechanism_test::cppelib_mechanism_test)

find_package(CppUTest REQUIRED)
target_link_libraries(${PROJECT_NAME} cpputest::cpputest)

if(MSVC)
	target_link_libraries(${PROJECT_NAME}
		-WHOLEARCHIVE:cppelib_mechanism_test.lib
	)
else()
	target_link_libraries(${PROJECT_NAME}
		-Wl,--whole-archive -lcppelib_mechanism_test -Wl,--no-whole-archive
	)
endif()
