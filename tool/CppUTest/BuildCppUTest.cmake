
if(NOT EXISTS ${CPPUTEST_DIR}/src)
	message("Start: Get CppUTest")
	execute_process(
		COMMAND git submodule update --init
	)
	message("Done: Get CppUTest")
endif()

message("Start: Build CppUTest")
execute_process(
	COMMAND ${CMAKE_COMMAND} ${CPPUTEST_DIR}
	WORKING_DIRECTORY ${CPPUTEST_BUILD_DIR}
)
execute_process(
	COMMAND ${CMAKE_MAKE_PROGRAM} CppUTest CppUTestExt
	WORKING_DIRECTORY ${CPPUTEST_BUILD_DIR}
)
message("Done: Build CppUTest")
