set(AssertionTestSrc
	${TEST_DIR}/AssertionTest/AssertionTest.cpp
)

set(ContainerTestSrc
	${TEST_DIR}/ContainerTest/ArrayTest.cpp
	${TEST_DIR}/ContainerTest/BitPatternTest.cpp
	${TEST_DIR}/ContainerTest/FixedDequeTest.cpp
	${TEST_DIR}/ContainerTest/FixedVectorTest.cpp
	${TEST_DIR}/ContainerTest/IntrusiveListTest.cpp
	${TEST_DIR}/ContainerTest/PreallocatedDequeTest.cpp
	${TEST_DIR}/ContainerTest/PreallocatedVectorTest.cpp
)

set(OSWrapperTestSrc
	${TEST_DIR}/OSWrapperTest/EventFlagTest.cpp
	${TEST_DIR}/OSWrapperTest/FixedMemoryPoolTest.cpp
	${TEST_DIR}/OSWrapperTest/MutexTest.cpp
	${TEST_DIR}/OSWrapperTest/OneShotTimerTest.cpp
	${TEST_DIR}/OSWrapperTest/PeriodicTimerTest.cpp
	${TEST_DIR}/OSWrapperTest/ThreadTest.cpp
	${TEST_DIR}/OSWrapperTest/VariableMemoryPoolTest.cpp
	${TEST_DIR}/OSWrapperTest/MessageQueueTest.cpp
	${TEST_DIR}/OSWrapperTest/ThreadPoolTest.cpp
)

add_executable(${PROJECT_NAME}
	${TEST_DIR}/main.cpp
	${AssertionTestSrc}
	${ContainerTestSrc}
	${OSWrapperTestSrc}
)
