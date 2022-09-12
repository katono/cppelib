include_directories(${CPPUTEST_DIR}/include)
include_directories(${MECHANISM_DIR})

link_directories(${CPPUTEST_BUILD_DIR}/src/CppUTest)
link_directories(${CPPUTEST_BUILD_DIR}/src/CppUTestExt)

set(AssertionTestSrc
	${MECHANISM_DIR}/test/AssertionTest/AssertionTest.cpp
)

set(ContainerTestSrc
	${MECHANISM_DIR}/test/ContainerTest/ArrayTest.cpp
	${MECHANISM_DIR}/test/ContainerTest/BitPatternTest.cpp
	${MECHANISM_DIR}/test/ContainerTest/FixedDequeTest.cpp
	${MECHANISM_DIR}/test/ContainerTest/FixedVectorTest.cpp
	${MECHANISM_DIR}/test/ContainerTest/IntrusiveListTest.cpp
	${MECHANISM_DIR}/test/ContainerTest/PreallocatedDequeTest.cpp
	${MECHANISM_DIR}/test/ContainerTest/PreallocatedVectorTest.cpp
)

set(OSWrapperSrc
	${MECHANISM_DIR}/OSWrapper/EventFlag.cpp
	${MECHANISM_DIR}/OSWrapper/FixedMemoryPool.cpp
	${MECHANISM_DIR}/OSWrapper/Mutex.cpp
	${MECHANISM_DIR}/OSWrapper/OneShotTimer.cpp
	${MECHANISM_DIR}/OSWrapper/PeriodicTimer.cpp
	${MECHANISM_DIR}/OSWrapper/Thread.cpp
	${MECHANISM_DIR}/OSWrapper/Timeout.cpp
	${MECHANISM_DIR}/OSWrapper/VariableMemoryPool.cpp
	${MECHANISM_DIR}/OSWrapper/ThreadPool.cpp
)

set(OSWrapperTestSrc
	${MECHANISM_DIR}/test/OSWrapperTest/EventFlagTest.cpp
	${MECHANISM_DIR}/test/OSWrapperTest/FixedMemoryPoolTest.cpp
	${MECHANISM_DIR}/test/OSWrapperTest/MutexTest.cpp
	${MECHANISM_DIR}/test/OSWrapperTest/OneShotTimerTest.cpp
	${MECHANISM_DIR}/test/OSWrapperTest/PeriodicTimerTest.cpp
	${MECHANISM_DIR}/test/OSWrapperTest/ThreadTest.cpp
	${MECHANISM_DIR}/test/OSWrapperTest/VariableMemoryPoolTest.cpp
	${MECHANISM_DIR}/test/OSWrapperTest/MessageQueueTest.cpp
	${MECHANISM_DIR}/test/OSWrapperTest/ThreadPoolTest.cpp
)

add_executable(alltests
	${MECHANISM_DIR}/test/main.cpp
	${AssertionTestSrc}
	${ContainerTestSrc}
	${OSWrapperSrc}
	${OSWrapperTestSrc}
)
