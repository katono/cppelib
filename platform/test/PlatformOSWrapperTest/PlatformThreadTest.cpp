#include "OSWrapper/Runnable.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/Mutex.h"
#include "Assertion/Assertion.h"
#include <stdexcept>

#include "PlatformOSWrapperTestHelper.h"
#if defined(PLATFORM_OS_WINDOWS)
#include <windows.h>
#include "WindowsOSWrapper/WindowsThreadFactory.h"
#elif defined(PLATFORM_OS_POSIX)
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include "PosixOSWrapper/PosixThreadFactory.h"
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
	Thread* thread;

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
		s_mutex = Mutex::create();
	}
	void teardown()
	{
		Mutex::destroy(s_mutex);
		PlatformOSWrapperTestHelper::destroyOSWrapperFactories();

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
#elif defined(PLATFORM_OS_STDCPP)
	LONGS_EQUAL(0, Thread::getMaxPriority());
	LONGS_EQUAL(0, Thread::getMinPriority());
	LONGS_EQUAL(0, Thread::getHighestPriority());
	LONGS_EQUAL(0, Thread::getLowestPriority());
#endif
}

class NativeHandleTestRunnable : public Runnable {
public:
	void run()
	{
		LockGuard lock(s_mutex);
		Thread* t = Thread::getCurrentThread();
#if defined(PLATFORM_OS_WINDOWS)
		LONGS_EQUAL(GetCurrentThreadId(), GetThreadId((HANDLE)t->getNativeHandle()));
#elif defined(PLATFORM_OS_POSIX)
		CHECK(pthread_equal(pthread_self(), reinterpret_cast<pthread_t>(t->getNativeHandle())) != 0);
#elif defined(PLATFORM_OS_STDCPP)
		CHECK(t->getNativeHandle());
#else
		(void) t->getNativeHandle();		
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

TEST(PlatformThreadTest, default_stackSize)
{
	StaticMethodTestRunnable runnable;
	thread = Thread::create(&runnable);

	CHECK(thread->getStackSize() > 0);
	Thread::destroy(thread);
}

#ifndef CPPELIB_NO_EXCEPTIONS
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
#endif

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

#if defined(PLATFORM_OS_WINDOWS)
TEST(PlatformThreadTest, setPriorityRange_highest_priority_is_max_value)
{
	OSWrapper::ThreadFactory* threadFactory = PlatformOSWrapperTestHelper::getThreadFactory();
	static_cast<WindowsOSWrapper::WindowsThreadFactory*>(threadFactory)->setPriorityRange(1, 9);

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
	OSWrapper::ThreadFactory* threadFactory = PlatformOSWrapperTestHelper::getThreadFactory();
	static_cast<WindowsOSWrapper::WindowsThreadFactory*>(threadFactory)->setPriorityRange(9, 1);

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
#elif defined(PLATFORM_OS_POSIX)
TEST(PlatformThreadTest, setPriorityRange_highest_priority_is_max_value)
{
	OSWrapper::ThreadFactory* threadFactory = PlatformOSWrapperTestHelper::getThreadFactory();
	static_cast<PosixOSWrapper::PosixThreadFactory*>(threadFactory)->setPriorityRange(1, 9);

	LONGS_EQUAL(1, Thread::getMinPriority());
	LONGS_EQUAL(9, Thread::getMaxPriority());
	LONGS_EQUAL(5, Thread::getNormalPriority());

	if (getuid() != 0) {
		// Not superuser
		return;
	}

	class CheckPrioRangeRunnable : public Runnable {
	public:
		void run()
		{
			LockGuard lock(s_mutex);
			Thread* t = Thread::getCurrentThread();
			int policy;
			struct sched_param param;
			int ret = 0;
			pthread_t pt = reinterpret_cast<pthread_t>(t->getNativeHandle());

			t->setPriority(Thread::getNormalPriority());
			ret = pthread_getschedparam(pt, &policy, &param);
			CHECK_FALSE(ret);
			LONGS_EQUAL(SCHED_OTHER, policy);
			LONGS_EQUAL(0, param.sched_priority);

			t->setPriority(Thread::getNormalPriority() + 1);
			ret = pthread_getschedparam(pt, &policy, &param);
			CHECK_FALSE(ret);
			LONGS_EQUAL(SCHED_RR, policy);
			LONGS_EQUAL(1, param.sched_priority);

			t->setPriority(Thread::getNormalPriority() + 2);
			ret = pthread_getschedparam(pt, &policy, &param);
			CHECK_FALSE(ret);
			LONGS_EQUAL(SCHED_RR, policy);
			LONGS_EQUAL(2, param.sched_priority);

			t->setPriority(Thread::getNormalPriority() + 3);
			ret = pthread_getschedparam(pt, &policy, &param);
			CHECK_FALSE(ret);
			LONGS_EQUAL(SCHED_RR, policy);
			LONGS_EQUAL(3, param.sched_priority);

			t->setPriority(Thread::getNormalPriority() - 1);
			ret = pthread_getschedparam(pt, &policy, &param);
			CHECK_FALSE(ret);
			LONGS_EQUAL(SCHED_OTHER, policy);
			LONGS_EQUAL(0, param.sched_priority);

			t->setPriority(Thread::getNormalPriority() - 2);
			ret = pthread_getschedparam(pt, &policy, &param);
			CHECK_FALSE(ret);
			LONGS_EQUAL(SCHED_OTHER, policy);
			LONGS_EQUAL(0, param.sched_priority);

			t->setPriority(Thread::getNormalPriority() - 3);
			ret = pthread_getschedparam(pt, &policy, &param);
			CHECK_FALSE(ret);
			LONGS_EQUAL(SCHED_OTHER, policy);
			LONGS_EQUAL(0, param.sched_priority);

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
	OSWrapper::ThreadFactory* threadFactory = PlatformOSWrapperTestHelper::getThreadFactory();
	static_cast<PosixOSWrapper::PosixThreadFactory*>(threadFactory)->setPriorityRange(9, 1);

	LONGS_EQUAL(1, Thread::getMinPriority());
	LONGS_EQUAL(9, Thread::getMaxPriority());
	LONGS_EQUAL(5, Thread::getNormalPriority());

	if (getuid() != 0) {
		// Not superuser
		return;
	}

	class CheckPrioRangeRunnable : public Runnable {
	public:
		void run()
		{
			LockGuard lock(s_mutex);
			Thread* t = Thread::getCurrentThread();
			int policy;
			struct sched_param param;
			int ret = 0;
			pthread_t pt = reinterpret_cast<pthread_t>(t->getNativeHandle());

			t->setPriority(Thread::getNormalPriority());
			ret = pthread_getschedparam(pt, &policy, &param);
			CHECK_FALSE(ret);
			LONGS_EQUAL(SCHED_OTHER, policy);
			LONGS_EQUAL(0, param.sched_priority);

			t->setPriority(Thread::getNormalPriority() - 1);
			ret = pthread_getschedparam(pt, &policy, &param);
			CHECK_FALSE(ret);
			LONGS_EQUAL(SCHED_RR, policy);
			LONGS_EQUAL(1, param.sched_priority);

			t->setPriority(Thread::getNormalPriority() - 2);
			ret = pthread_getschedparam(pt, &policy, &param);
			CHECK_FALSE(ret);
			LONGS_EQUAL(SCHED_RR, policy);
			LONGS_EQUAL(2, param.sched_priority);

			t->setPriority(Thread::getNormalPriority() - 3);
			ret = pthread_getschedparam(pt, &policy, &param);
			CHECK_FALSE(ret);
			LONGS_EQUAL(SCHED_RR, policy);
			LONGS_EQUAL(3, param.sched_priority);

			t->setPriority(Thread::getNormalPriority() + 1);
			ret = pthread_getschedparam(pt, &policy, &param);
			CHECK_FALSE(ret);
			LONGS_EQUAL(SCHED_OTHER, policy);
			LONGS_EQUAL(0, param.sched_priority);

			t->setPriority(Thread::getNormalPriority() + 2);
			ret = pthread_getschedparam(pt, &policy, &param);
			CHECK_FALSE(ret);
			LONGS_EQUAL(SCHED_OTHER, policy);
			LONGS_EQUAL(0, param.sched_priority);

			t->setPriority(Thread::getNormalPriority() + 3);
			ret = pthread_getschedparam(pt, &policy, &param);
			CHECK_FALSE(ret);
			LONGS_EQUAL(SCHED_OTHER, policy);
			LONGS_EQUAL(0, param.sched_priority);

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
