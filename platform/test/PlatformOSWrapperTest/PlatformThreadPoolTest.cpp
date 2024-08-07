#include "OSWrapper/Runnable.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/Mutex.h"
#include "OSWrapper/EventFlag.h"
#include "OSWrapper/FixedMemoryPool.h"
#include "OSWrapper/ThreadPool.h"
#include "Assertion/Assertion.h"
#include <stdexcept>

#include "PlatformOSWrapperTestHelper.h"
#if defined(PLATFORM_OS_WINDOWS)
#include "WindowsOSWrapper/WindowsThreadFactory.h"
#elif defined(PLATFORM_OS_POSIX)
#include "PosixOSWrapper/PosixThreadFactory.h"
#endif

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

namespace PlatformThreadPoolTest {

using OSWrapper::Runnable;
using OSWrapper::Thread;
using OSWrapper::ThreadPool;
using OSWrapper::Timeout;


TEST_GROUP(PlatformThreadPoolTest) {
	void setup()
	{
		PlatformOSWrapperTestHelper::createAndRegisterOSWrapperFactories();
#if defined(PLATFORM_OS_WINDOWS)
		OSWrapper::ThreadFactory* threadFactory = PlatformOSWrapperTestHelper::getThreadFactory();
		static_cast<WindowsOSWrapper::WindowsThreadFactory*>(threadFactory)->setPriorityRange(1, 9);
#elif defined(PLATFORM_OS_POSIX)
		OSWrapper::ThreadFactory* threadFactory = PlatformOSWrapperTestHelper::getThreadFactory();
		static_cast<PosixOSWrapper::PosixThreadFactory*>(threadFactory)->setPriorityRange(1, 9);
#endif
	}
	void teardown()
	{
		PlatformOSWrapperTestHelper::destroyOSWrapperFactories();

		mock().checkExpectations();
		mock().clear();
	}
};

class TestRunnable : public Runnable {
	int m_result;
public:
	explicit TestRunnable(int input = 0) : m_result(input) {}
	int getResult() const { return m_result; }
	void run()
	{
		m_result += 100;
	}
};


TEST(PlatformThreadPoolTest, create_destroy)
{
	ThreadPool* threadPool = ThreadPool::create(10, 4096);
	CHECK(threadPool);
	ThreadPool::destroy(threadPool);
}

TEST(PlatformThreadPoolTest, destroy_nullptr)
{
	ThreadPool::destroy(0);
}

TEST(PlatformThreadPoolTest, start_nowaiter)
{
	TestRunnable runnable(1);
	{
		ThreadPool* threadPool = ThreadPool::create(10, 4096);
		CHECK(threadPool);

		OSWrapper::Error err = threadPool->start(&runnable);
		LONGS_EQUAL(OSWrapper::OK, err);

		ThreadPool::destroy(threadPool);
	}
	LONGS_EQUAL(101, runnable.getResult());
}

TEST(PlatformThreadPoolTest, start_waiter_auto_release_by_dtor)
{
	ThreadPool* threadPool = ThreadPool::create(10, 4096);
	CHECK(threadPool);

	TestRunnable runnable(1);
	{
		ThreadPool::WaitGuard waiter;
		OSWrapper::Error err = threadPool->start(&runnable, &waiter);
		LONGS_EQUAL(OSWrapper::OK, err);

		err = waiter.wait();
		LONGS_EQUAL(OSWrapper::OK, err);
	}
	LONGS_EQUAL(101, runnable.getResult());

	ThreadPool::destroy(threadPool);
}

TEST(PlatformThreadPoolTest, start_waiter_auto_wait_release_by_dtor)
{
	ThreadPool* threadPool = ThreadPool::create(10, 4096);
	CHECK(threadPool);

	TestRunnable runnable(1);
	{
		ThreadPool::WaitGuard waiter;
		OSWrapper::Error err = threadPool->start(&runnable, &waiter);
		LONGS_EQUAL(OSWrapper::OK, err);
	}
	LONGS_EQUAL(101, runnable.getResult());

	ThreadPool::destroy(threadPool);
}

TEST(PlatformThreadPoolTest, start_waiter_manual_release)
{
	ThreadPool* threadPool = ThreadPool::create(10, 4096);
	CHECK(threadPool);

	TestRunnable runnable(1);
	ThreadPool::WaitGuard waiter;
	OSWrapper::Error err = threadPool->start(&runnable, &waiter);
	LONGS_EQUAL(OSWrapper::OK, err);

	waiter.release();
	LONGS_EQUAL(101, runnable.getResult());

	ThreadPool::destroy(threadPool);
}

TEST(PlatformThreadPoolTest, start_waiter_multi)
{
	ThreadPool* threadPool = ThreadPool::create(10, 4096);
	CHECK(threadPool);

	TestRunnable runnable1(1);
	TestRunnable runnable2(2);
	TestRunnable runnable3(3);
	{
		OSWrapper::Error err;
		ThreadPool::WaitGuard waiter1;
		err = threadPool->start(&runnable1, &waiter1);
		LONGS_EQUAL(OSWrapper::OK, err);

		ThreadPool::WaitGuard waiter2;
		err = threadPool->start(&runnable2, &waiter2);
		LONGS_EQUAL(OSWrapper::OK, err);

		ThreadPool::WaitGuard waiter3;
		err = threadPool->start(&runnable3, &waiter3);
		LONGS_EQUAL(OSWrapper::OK, err);

		waiter1.wait();
		LONGS_EQUAL(101, runnable1.getResult());

		waiter2.wait();
		LONGS_EQUAL(102, runnable2.getResult());

		waiter3.wait();
		LONGS_EQUAL(103, runnable3.getResult());
	}

	ThreadPool::destroy(threadPool);
}

class SleepRunnable : public Runnable {
	unsigned long m_millis;
public:
	explicit SleepRunnable(unsigned long millis) : m_millis(millis) {}
	void run()
	{
		Thread::sleep(m_millis);
	}
};

TEST(PlatformThreadPoolTest, start_waiter_tryWait)
{
	ThreadPool* threadPool = ThreadPool::create(10, 4096);
	CHECK(threadPool);

	SleepRunnable runnable(10);
	{
		ThreadPool::WaitGuard waiter;
		OSWrapper::Error err = threadPool->start(&runnable, &waiter);
		LONGS_EQUAL(OSWrapper::OK, err);

		do {
			err = waiter.tryWait();
			Thread::yield();
		} while (err != OSWrapper::OK);
	}

	ThreadPool::destroy(threadPool);
}

TEST(PlatformThreadPoolTest, start_waiter_timedWait)
{
	ThreadPool* threadPool = ThreadPool::create(10, 4096);
	CHECK(threadPool);

	SleepRunnable runnable(50);
	{
		ThreadPool::WaitGuard waiter;
		OSWrapper::Error err = threadPool->start(&runnable, &waiter);
		LONGS_EQUAL(OSWrapper::OK, err);

		err = waiter.timedWait(Timeout(1));
		LONGS_EQUAL(OSWrapper::TimedOut, err);

		err = waiter.timedWait(Timeout(100));
		LONGS_EQUAL(OSWrapper::OK, err);
	}

	ThreadPool::destroy(threadPool);
}


#ifndef CPPELIB_NO_EXCEPTIONS
class ExceptionRunnable : public Runnable {
	class OtherTask : public Runnable {
		void run()
		{
			Thread::sleep(1);
		}
	};
	OtherTask m_task;
	ThreadPool* m_tp;
public:
	explicit ExceptionRunnable(ThreadPool* tp) :m_tp(tp) {}
	void run()
	{
		m_tp->start(&m_task);
		throw std::runtime_error("Exception Test");
	}
};

class MyExceptionHandler : public Thread::UncaughtExceptionHandler {
	const char* m_expectedMsg;
public:
	explicit MyExceptionHandler(const char* expectedMsg) : m_expectedMsg(expectedMsg) {}
	virtual void handle(Thread*, const char* expectedMsg)
	{
		mock().actualCall("handle").onObject(this);
		STRCMP_CONTAINS(m_expectedMsg, expectedMsg);
	}
};

TEST(PlatformThreadPoolTest, exception)
{
	ThreadPool* threadPool = ThreadPool::create(10, 4096);
	CHECK(threadPool);
	MyExceptionHandler handler("Exception Test");
	threadPool->setUncaughtExceptionHandler(&handler);
	POINTERS_EQUAL(&handler, threadPool->getUncaughtExceptionHandler());
	mock().expectOneCall("handle").onObject(&handler);

	ExceptionRunnable runnable(threadPool);
	{
		ThreadPool::WaitGuard waiter;
		OSWrapper::Error err = threadPool->start(&runnable, &waiter);
		LONGS_EQUAL(OSWrapper::OK, err);

		waiter.wait();
	}

	ThreadPool::destroy(threadPool);
}
#endif

TEST(PlatformThreadPoolTest, start_runnable_nullptr)
{
	ThreadPool* threadPool = ThreadPool::create(10, 4096);
	CHECK(threadPool);

	OSWrapper::Error err = threadPool->start(0, 0);
	LONGS_EQUAL(OSWrapper::InvalidParameter, err);

	ThreadPool::destroy(threadPool);
}

TEST(PlatformThreadPoolTest, tryStart_failed)
{
	ThreadPool* threadPool = ThreadPool::create(10, 4096);
	CHECK(threadPool);

	const int num = 100;
	struct Data {
		TestRunnable runnable;
		ThreadPool::WaitGuard waiter;
		OSWrapper::Error err;
		Data() : runnable(), waiter(), err(OSWrapper::OtherError) {}
	} data[num];

	for (int i = 0; i < num; i++) {
		data[i].err = threadPool->tryStart(&data[i].runnable, &data[i].waiter);
	}
	bool failed = false;
	for (int i = 0; i < num; i++) {
		if (data[i].err == OSWrapper::OK) {
			CHECK(data[i].waiter.isValid());
			data[i].waiter.wait();
			data[i].waiter.release();
		} else {
			CHECK(!data[i].waiter.isValid());
			failed = true;
		}
	}
	CHECK(failed);

	ThreadPool::destroy(threadPool);
}

TEST(PlatformThreadPoolTest, check_priority_stackSize_threadName)
{
	class CheckPriorityStackSizeRunnable : public Runnable {
	public:
		Thread* m_thread;
		void run()
		{
			m_thread = Thread::getCurrentThread();
			LONGS_EQUAL(4096, m_thread->getStackSize());
			LONGS_EQUAL(Thread::getPriorityHigherThan(Thread::getNormalPriority(), 1),
					m_thread->getPriority());
			STRCMP_EQUAL("ThreadPool", m_thread->getName());
		}
	};
	ThreadPool* threadPool = ThreadPool::create(10, 4096, Thread::getHighestPriority(), "ThreadPool");
	CHECK(threadPool);
	STRCMP_EQUAL("ThreadPool", threadPool->getThreadName());

	CheckPriorityStackSizeRunnable runnable;
	{
		ThreadPool::WaitGuard waiter;
		OSWrapper::Error err = threadPool->start(&runnable, &waiter,
				Thread::getPriorityHigherThan(Thread::getNormalPriority(), 1));
		LONGS_EQUAL(OSWrapper::OK, err);
	}
	LONGS_EQUAL(Thread::getHighestPriority(), runnable.m_thread->getPriority());

	ThreadPool::destroy(threadPool);
}

TEST(PlatformThreadPoolTest, invalid_waiter)
{
	ThreadPool::WaitGuard waiter;
	CHECK(!waiter.isValid());

	OSWrapper::Error err;
	err = waiter.wait();
	LONGS_EQUAL(OSWrapper::OK, err);

	err = waiter.tryWait();
	LONGS_EQUAL(OSWrapper::OK, err);

	err = waiter.timedWait(Timeout(100));
	LONGS_EQUAL(OSWrapper::OK, err);
}

} // namespace PlatformThreadPoolTest
