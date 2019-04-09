#include "OSWrapper/Runnable.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/Mutex.h"
#include "Assertion/Assertion.h"
#include <stdexcept>

#ifdef PLATFORM_OS_WINDOWS
#include <windows.h>
#include "WindowsOSWrapper/WindowsThreadFactory.h"
#include "WindowsOSWrapper/WindowsMutexFactory.h"
typedef WindowsOSWrapper::WindowsThreadFactory PlatformThreadFactory;
typedef WindowsOSWrapper::WindowsMutexFactory PlatformMutexFactory;
#elif PLATFORM_OS_POSIX
#include <pthread.h>
#include "PosixOSWrapper/PosixThreadFactory.h"
#include "PosixOSWrapper/PosixMutexFactory.h"
typedef PosixOSWrapper::PosixThreadFactory PlatformThreadFactory;
typedef PosixOSWrapper::PosixMutexFactory PlatformMutexFactory;
#elif PLATFORM_OS_STDCPP
#include "StdCppOSWrapper/StdCppThreadFactory.h"
#include "StdCppOSWrapper/StdCppMutexFactory.h"
typedef StdCppOSWrapper::StdCppThreadFactory PlatformThreadFactory;
typedef StdCppOSWrapper::StdCppMutexFactory PlatformMutexFactory;
#elif PLATFORM_OS_ITRON
#include "ItronOSWrapper/ItronThreadFactory.h"
#include "ItronOSWrapper/ItronMutexFactory.h"
typedef ItronOSWrapper::ItronThreadFactory PlatformThreadFactory;
typedef ItronOSWrapper::ItronMutexFactory PlatformMutexFactory;
#endif

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

namespace PlatformThreadTest {

using OSWrapper::Runnable;
using OSWrapper::Thread;
using OSWrapper::Mutex;
using OSWrapper::LockGuard;

static Mutex* s_mutex;

TEST_GROUP(PlatformThreadTest) {
	PlatformThreadFactory testThreadFactory;
	PlatformMutexFactory testMutexFactory;

	Thread* thread;

	void setup()
	{
#if defined(PLATFORM_OS_WINDOWS) || defined(PLATFORM_OS_POSIX)
		testThreadFactory.setPriorityRange(1, 9);
#endif
		OSWrapper::registerMutexFactory(&testMutexFactory);
		OSWrapper::registerThreadFactory(&testThreadFactory);

		s_mutex = Mutex::create();
	}
	void teardown()
	{
		Mutex::destroy(s_mutex);

		mock().checkExpectations();
		mock().clear();
	}
};

class StaticMethodTestRunnable : public Runnable {
private:
	Thread* m_thread;
public:
	void setThread(Thread* t)
	{
		m_thread = t;
	}
	void run()
	{
		LockGuard lock(s_mutex);
		Thread* t = Thread::getCurrentThread();
		POINTERS_EQUAL(m_thread, t);
		Thread::yield();
		Thread::sleep(100);
		Thread::exit();
		FAIL("failed");
	}
};


TEST(PlatformThreadTest, create_destroy)
{
	StaticMethodTestRunnable runnable;
	thread = Thread::create(&runnable, Thread::getNormalPriority(), 4096, 0, "TestThread");
	CHECK(thread);
	Thread::destroy(thread);
}

TEST(PlatformThreadTest, start_wait_isFinished)
{
	StaticMethodTestRunnable runnable;
	thread = Thread::create(&runnable, Thread::getNormalPriority(), 4096, 0, "TestThread");
	runnable.setThread(thread);
	thread->start();

	CHECK(!thread->isFinished());

	OSWrapper::Error err = thread->tryWait();
	LONGS_EQUAL(OSWrapper::TimedOut, err);

	err = thread->timedWait(OSWrapper::Timeout(10));
	LONGS_EQUAL(OSWrapper::TimedOut, err);

	err = thread->wait();
	LONGS_EQUAL(OSWrapper::OK, err);

	CHECK(thread->isFinished());

	Thread::destroy(thread);
}

TEST(PlatformThreadTest, create_failed_runnable_nullptr)
{
	thread = Thread::create(0, Thread::getNormalPriority(), 4096, 0, "TestThread");
	CHECK_FALSE(thread);
}

class MockRunnable : public Runnable {
public:
	void run()
	{
		{
			LockGuard lock(s_mutex);
			mock().actualCall("run");
		}
		Thread::yield();
		Thread::sleep(10);
	}
};

TEST(PlatformThreadTest, many_threads)
{
	const int num = 100;
	MockRunnable runnable[num];
	mock().expectNCalls(num, "run");

	Thread* t[num];
	for (int i = 0; i < num; i++) {
		t[i] = Thread::create(&runnable[i], Thread::getNormalPriority());
	}

	for (int i = 0; i < num; i++) {
		t[i]->start();
	}

	for (int i = 0; i < num; i++) {
		t[i]->wait();
	}

	for (int i = 0; i < num; i++) {
		Thread::destroy(t[i]);
	}
}

TEST(PlatformThreadTest, repeat_start)
{
	MockRunnable runnable;
	thread = Thread::create(&runnable, Thread::getNormalPriority());
	mock().expectNCalls(2, "run");

	thread->start();
	thread->start();
	thread->wait();
	Thread::sleep(10);

	thread->start();
	thread->start();
	thread->wait();

	Thread::destroy(thread);
}

TEST(PlatformThreadTest, name)
{
	StaticMethodTestRunnable runnable;
	thread = Thread::create(&runnable, Thread::getNormalPriority(), 4096, 0, "TestThread");

	STRCMP_EQUAL("TestThread", thread->getName());
	thread->setName("foo");
	STRCMP_EQUAL("foo", thread->getName());
	Thread::destroy(thread);
}

TEST(PlatformThreadTest, priority)
{
	StaticMethodTestRunnable runnable;
	thread = Thread::create(&runnable, 1, 4096, 0, "TestThread");
	LONGS_EQUAL(1, thread->getPriority());
	thread->setPriority(2);
	LONGS_EQUAL(2, thread->getPriority());

	runnable.setThread(thread);
	thread->start();
	const int prio = Thread::getNormalPriority();
	thread->setPriority(prio);
	LONGS_EQUAL(prio, thread->getPriority());
	LONGS_EQUAL(1, thread->getInitialPriority());
	Thread::destroy(thread);
}

TEST(PlatformThreadTest, priority_max_min_highest_lowest)
{
#if defined(PLATFORM_OS_WINDOWS) || defined(PLATFORM_OS_POSIX)
	LONGS_EQUAL(9, Thread::getMaxPriority());
	LONGS_EQUAL(1, Thread::getMinPriority());
	LONGS_EQUAL(9, Thread::getHighestPriority());
	LONGS_EQUAL(1, Thread::getLowestPriority());
#elif PLATFORM_OS_ITRON
	LONGS_EQUAL(TMAX_TPRI, Thread::getMaxPriority());
	LONGS_EQUAL(TMIN_TPRI, Thread::getMinPriority());
	LONGS_EQUAL(TMIN_TPRI, Thread::getHighestPriority());
	LONGS_EQUAL(TMAX_TPRI, Thread::getLowestPriority());
#endif
}

class NativeHandleTestRunnable : public Runnable {
public:
	void run()
	{
		LockGuard lock(s_mutex);
		Thread* t = Thread::getCurrentThread();
#ifdef PLATFORM_OS_WINDOWS
		LONGS_EQUAL(GetCurrentThreadId(), GetThreadId((HANDLE)t->getNativeHandle()));
#elif PLATFORM_OS_POSIX
		CHECK(pthread_equal(pthread_self(), reinterpret_cast<pthread_t>(t->getNativeHandle())) != 0);
#elif PLATFORM_OS_STDCPP
		CHECK(t->getNativeHandle());
#elif PLATFORM_OS_ITRON
		ID tskid = (ID)t->getNativeHandle();
		T_RTSK rtsk = {0};
		ER err = ref_tsk(tskid, &rtsk);
		LONGS_EQUAL(E_OK, err);
		LONGS_EQUAL(TTS_RUN, rtsk.tskstat);
#endif
	}
};

TEST(PlatformThreadTest, getNativeHandle)
{
	NativeHandleTestRunnable runnable;
	thread = Thread::create(&runnable, Thread::getNormalPriority());
	thread->start();

	thread->wait();
	Thread::destroy(thread);
}

TEST(PlatformThreadTest, stackSize)
{
	StaticMethodTestRunnable runnable;
	thread = Thread::create(&runnable, Thread::getNormalPriority(), 4096, 0, "TestThread");

	LONGS_EQUAL(4096, thread->getStackSize());
	Thread::destroy(thread);
}

class ThrowExceptionRunnable : public Runnable {
public:
	enum Kind {
		Std,
		Assert,
		Integer,
	};
	ThrowExceptionRunnable(Kind kind) : m_kind(kind) {}
	void run()
	{
		LockGuard lock(s_mutex);
		switch (m_kind) {
		case Std:
			throw std::runtime_error("Exception Test");
			break;
		case Assert:
			CHECK_ASSERT("CHECK_ASSERT_EXCEPTION_TEST" && false);
			break;
		case Integer:
			throw 1;
			break;
		}
	}
private:
	Kind m_kind;
};

class MyExceptionHandler : public Thread::UncaughtExceptionHandler {
	const char* m_expectedMsg;
public:
	explicit MyExceptionHandler(const char* expectedMsg) : m_expectedMsg(expectedMsg) {}
	virtual void handle(Thread* t, const char* expectedMsg)
	{
		mock().actualCall("handle").withParameter("t", t).onObject(this);
		STRCMP_CONTAINS(m_expectedMsg, expectedMsg);
	}
};

TEST(PlatformThreadTest, exception_std)
{
	ThrowExceptionRunnable runnable(ThrowExceptionRunnable::Std);
	thread = Thread::create(&runnable, Thread::getNormalPriority());

	MyExceptionHandler handler("Exception Test");
	thread->setUncaughtExceptionHandler(&handler);
	mock().expectOneCall("handle").withParameter("t", thread).onObject(&handler);

	thread->start();
	Thread::destroy(thread);
}

TEST(PlatformThreadTest, exception_assert)
{
	ThrowExceptionRunnable runnable(ThrowExceptionRunnable::Assert);
	thread = Thread::create(&runnable, Thread::getNormalPriority());

	MyExceptionHandler handler("CHECK_ASSERT_EXCEPTION_TEST");
	thread->setUncaughtExceptionHandler(&handler);
	mock().expectOneCall("handle").withParameter("t", thread).onObject(&handler);

	thread->start();
	Thread::destroy(thread);
}

TEST(PlatformThreadTest, exception_unknown)
{
	ThrowExceptionRunnable runnable(ThrowExceptionRunnable::Integer);
	thread = Thread::create(&runnable, Thread::getNormalPriority());

	MyExceptionHandler handler("Unknown Exception");
	thread->setUncaughtExceptionHandler(&handler);
	mock().expectOneCall("handle").withParameter("t", thread).onObject(&handler);

	thread->start();
	Thread::destroy(thread);
}

TEST(PlatformThreadTest, setPriority_inherit)
{
	class Child : public Runnable {
	public:
		void run()
		{
			LockGuard lock(s_mutex);
			Thread* t = Thread::getCurrentThread();
			LONGS_EQUAL(Thread::getNormalPriority() + 1, t->getPriority());
		}
	};
	class Parent : public Runnable {
	public:
		void run()
		{
			Child runnable;
			Thread* t = Thread::create(&runnable, Thread::INHERIT_PRIORITY);
			{
				LockGuard lock(s_mutex);
				CHECK(t);
			}
			t->start();
			Thread::destroy(t);
		}
	};
	Parent runnable;
	thread = Thread::create(&runnable, Thread::getNormalPriority() + 1);
	CHECK(thread);
	thread->start();
	Thread::destroy(thread);
}

#ifdef PLATFORM_OS_WINDOWS
TEST(PlatformThreadTest, setPriorityRange_highest_priority_is_max_value)
{
	testThreadFactory.setPriorityRange(1, 9);
	LONGS_EQUAL(1, Thread::getMinPriority());
	LONGS_EQUAL(9, Thread::getMaxPriority());
	LONGS_EQUAL(5, Thread::getNormalPriority());

	class CheckPrioRangeRunnable : public Runnable {
	public:
		void run()
		{
			LockGuard lock(s_mutex);
			Thread* t = Thread::getCurrentThread();
			int winPriority;

			t->setPriority(Thread::getNormalPriority());
			winPriority = GetThreadPriority(GetCurrentThread());
			LONGS_EQUAL(THREAD_PRIORITY_NORMAL, winPriority);

			t->setPriority(Thread::getNormalPriority() + 1);
			winPriority = GetThreadPriority(GetCurrentThread());
			LONGS_EQUAL(THREAD_PRIORITY_ABOVE_NORMAL, winPriority);

			t->setPriority(Thread::getNormalPriority() + 2);
			winPriority = GetThreadPriority(GetCurrentThread());
			LONGS_EQUAL(THREAD_PRIORITY_HIGHEST, winPriority);

			t->setPriority(Thread::getNormalPriority() + 3);
			winPriority = GetThreadPriority(GetCurrentThread());
			LONGS_EQUAL(THREAD_PRIORITY_HIGHEST, winPriority);

			t->setPriority(Thread::getMaxPriority());
			winPriority = GetThreadPriority(GetCurrentThread());
			LONGS_EQUAL(THREAD_PRIORITY_TIME_CRITICAL, winPriority);

			t->setPriority(Thread::getNormalPriority() - 1);
			winPriority = GetThreadPriority(GetCurrentThread());
			LONGS_EQUAL(THREAD_PRIORITY_BELOW_NORMAL, winPriority);

			t->setPriority(Thread::getNormalPriority() - 2);
			winPriority = GetThreadPriority(GetCurrentThread());
			LONGS_EQUAL(THREAD_PRIORITY_LOWEST, winPriority);

			t->setPriority(Thread::getNormalPriority() - 3);
			winPriority = GetThreadPriority(GetCurrentThread());
			LONGS_EQUAL(THREAD_PRIORITY_LOWEST, winPriority);

			t->setPriority(Thread::getMinPriority());
			winPriority = GetThreadPriority(GetCurrentThread());
			LONGS_EQUAL(THREAD_PRIORITY_IDLE, winPriority);

		}
	};

	CheckPrioRangeRunnable runnable;
	thread = Thread::create(&runnable, Thread::getNormalPriority());
	CHECK(thread);
	thread->start();
	Thread::destroy(thread);
}

TEST(PlatformThreadTest, setPriorityRange_highest_priority_is_min_value)
{
	testThreadFactory.setPriorityRange(9, 1);
	LONGS_EQUAL(1, Thread::getMinPriority());
	LONGS_EQUAL(9, Thread::getMaxPriority());
	LONGS_EQUAL(5, Thread::getNormalPriority());

	class CheckPrioRangeRunnable : public Runnable {
	public:
		void run()
		{
			LockGuard lock(s_mutex);
			Thread* t = Thread::getCurrentThread();
			int winPriority;

			t->setPriority(Thread::getNormalPriority());
			winPriority = GetThreadPriority(GetCurrentThread());
			LONGS_EQUAL(THREAD_PRIORITY_NORMAL, winPriority);

			t->setPriority(Thread::getNormalPriority() - 1);
			winPriority = GetThreadPriority(GetCurrentThread());
			LONGS_EQUAL(THREAD_PRIORITY_ABOVE_NORMAL, winPriority);

			t->setPriority(Thread::getNormalPriority() - 2);
			winPriority = GetThreadPriority(GetCurrentThread());
			LONGS_EQUAL(THREAD_PRIORITY_HIGHEST, winPriority);

			t->setPriority(Thread::getNormalPriority() - 3);
			winPriority = GetThreadPriority(GetCurrentThread());
			LONGS_EQUAL(THREAD_PRIORITY_HIGHEST, winPriority);

			t->setPriority(Thread::getMinPriority());
			winPriority = GetThreadPriority(GetCurrentThread());
			LONGS_EQUAL(THREAD_PRIORITY_TIME_CRITICAL, winPriority);

			t->setPriority(Thread::getNormalPriority() + 1);
			winPriority = GetThreadPriority(GetCurrentThread());
			LONGS_EQUAL(THREAD_PRIORITY_BELOW_NORMAL, winPriority);

			t->setPriority(Thread::getNormalPriority() + 2);
			winPriority = GetThreadPriority(GetCurrentThread());
			LONGS_EQUAL(THREAD_PRIORITY_LOWEST, winPriority);

			t->setPriority(Thread::getNormalPriority() + 3);
			winPriority = GetThreadPriority(GetCurrentThread());
			LONGS_EQUAL(THREAD_PRIORITY_LOWEST, winPriority);

			t->setPriority(Thread::getMaxPriority());
			winPriority = GetThreadPriority(GetCurrentThread());
			LONGS_EQUAL(THREAD_PRIORITY_IDLE, winPriority);

		}
	};

	CheckPrioRangeRunnable runnable;
	thread = Thread::create(&runnable, Thread::getNormalPriority());
	CHECK(thread);
	thread->start();
	Thread::destroy(thread);
}
#endif

} // namespace PlatformThreadTest
