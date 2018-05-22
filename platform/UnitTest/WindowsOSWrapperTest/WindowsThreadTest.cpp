#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "OSWrapper/Runnable.h"
#include "OSWrapper/Thread.h"
#include "WindowsOSWrapper/WindowsThreadFactory.h"
#include "Assertion/Assertion.h"
#include <mutex>
#include <stdexcept>
#include <windows.h>

using OSWrapper::Runnable;
using OSWrapper::Thread;
using WindowsOSWrapper::WindowsThreadFactory;

static std::mutex s_mutex;

TEST_GROUP(WindowsThreadTest) {
	WindowsThreadFactory testFactory;
	Thread* thread;

	void setup()
	{
		testFactory.setPriorityRange(1, 9);
		OSWrapper::registerThreadFactory(&testFactory);
	}
	void teardown()
	{
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
		std::lock_guard<std::mutex> lock(s_mutex);
		Thread* t = Thread::getCurrentThread();
		POINTERS_EQUAL(m_thread, t);
		Thread::yield();
		Thread::sleep(100);
		Thread::exit();
		FAIL("failed");
	}
};


TEST(WindowsThreadTest, create_destroy)
{
	StaticMethodTestRunnable runnable;
	thread = Thread::create(&runnable, Thread::getNormalPriority(), 4096, 0, "WindowsThread");
	CHECK(thread);
	Thread::destroy(thread);
}

TEST(WindowsThreadTest, start_wait_isFinished)
{
	StaticMethodTestRunnable runnable;
	thread = Thread::create(&runnable, Thread::getNormalPriority(), 4096, 0, "WindowsThread");
	runnable.setThread(thread);
	thread->start();

	CHECK(!thread->isFinished());

	thread->wait();
	CHECK(thread->isFinished());

	Thread::destroy(thread);
}

TEST(WindowsThreadTest, create_failed_runnable_nullptr)
{
	thread = Thread::create(0, Thread::getNormalPriority(), 4096, 0, "WindowsThread");
	CHECK_FALSE(thread);
}

class MockRunnable : public Runnable {
public:
	void run()
	{
		{
			std::lock_guard<std::mutex> lock(s_mutex);
			mock().actualCall("run");
		}
		Thread::yield();
		Thread::sleep(10);
	}
};

TEST(WindowsThreadTest, many_threads)
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

TEST(WindowsThreadTest, repeat_start)
{
	MockRunnable runnable;
	thread = Thread::create(&runnable, Thread::getNormalPriority());
	mock().expectNCalls(2, "run");

	thread->start();
	thread->start();
	thread->wait();

	thread->start();
	thread->start();
	thread->wait();

	Thread::destroy(thread);
}

TEST(WindowsThreadTest, name)
{
	StaticMethodTestRunnable runnable;
	thread = Thread::create(&runnable, Thread::getNormalPriority(), 4096, 0, "TestThread");

	STRCMP_EQUAL("TestThread", thread->getName());
	thread->setName("foo");
	STRCMP_EQUAL("foo", thread->getName());
	Thread::destroy(thread);
}

TEST(WindowsThreadTest, priority)
{
	StaticMethodTestRunnable runnable;
	thread = Thread::create(&runnable, 1, 4096, 0, "TestThread");

	LONGS_EQUAL(1, thread->getPriority());
	const int prio = (Thread::getMaxPriority() + Thread::getMinPriority()) / 2;
	thread->setPriority(prio);
	LONGS_EQUAL(prio, thread->getPriority());
	Thread::destroy(thread);
}

class NativeHandleTestRunnable : public Runnable {
public:
	void run()
	{
		std::lock_guard<std::mutex> lock(s_mutex);
		Thread* t = Thread::getCurrentThread();
		LONGS_EQUAL(GetCurrentThreadId(), GetThreadId((HANDLE)t->getNativeHandle()));
	}
};

TEST(WindowsThreadTest, getNativeHandle)
{
	NativeHandleTestRunnable runnable;
	thread = Thread::create(&runnable, Thread::getNormalPriority());
	thread->start();

	thread->wait();
	Thread::destroy(thread);
}

TEST(WindowsThreadTest, stackSize)
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
		std::lock_guard<std::mutex> lock(s_mutex);
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

class MyExceptionHandler : public Thread::ExceptionHandler {
public:
	virtual void handle(Thread* t, const std::exception&)
	{
		mock().actualCall("handle").withParameter("t", t).onObject(this);
	}
};

TEST(WindowsThreadTest, exception_std)
{
	ThrowExceptionRunnable runnable(ThrowExceptionRunnable::Std);
	thread = Thread::create(&runnable, Thread::getNormalPriority());

	MyExceptionHandler handler;
	thread->setExceptionHandler(&handler);
	mock().expectOneCall("handle").withParameter("t", thread).onObject(&handler);

	thread->start();
	Thread::destroy(thread);
}

TEST(WindowsThreadTest, exception_assert)
{
	ThrowExceptionRunnable runnable(ThrowExceptionRunnable::Assert);
	thread = Thread::create(&runnable, Thread::getNormalPriority());

	MyExceptionHandler handler;
	thread->setExceptionHandler(&handler);
	mock().expectOneCall("handle").withParameter("t", thread).onObject(&handler);

	thread->start();
	Thread::destroy(thread);
}

TEST(WindowsThreadTest, exception_unknown)
{
	ThrowExceptionRunnable runnable(ThrowExceptionRunnable::Integer);
	thread = Thread::create(&runnable, Thread::getNormalPriority());

	MyExceptionHandler handler;
	thread->setExceptionHandler(&handler);
	mock().expectOneCall("handle").withParameter("t", thread).onObject(&handler);

	thread->start();
	Thread::destroy(thread);
}

TEST(WindowsThreadTest, setPriorityRange_highest_priority_is_max_value)
{
	testFactory.setPriorityRange(1, 9);
	LONGS_EQUAL(1, Thread::getMinPriority());
	LONGS_EQUAL(9, Thread::getMaxPriority());
	LONGS_EQUAL(5, Thread::getNormalPriority());

	class CheckPrioRangeRunnable : public Runnable {
	public:
		void run()
		{
			std::lock_guard<std::mutex> lock(s_mutex);
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

TEST(WindowsThreadTest, setPriorityRange_highest_priority_is_min_value)
{
	testFactory.setPriorityRange(9, 1);
	LONGS_EQUAL(1, Thread::getMinPriority());
	LONGS_EQUAL(9, Thread::getMaxPriority());
	LONGS_EQUAL(5, Thread::getNormalPriority());

	class CheckPrioRangeRunnable : public Runnable {
	public:
		void run()
		{
			std::lock_guard<std::mutex> lock(s_mutex);
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

TEST(WindowsThreadTest, setPriority_inherit)
{
	class Child : public Runnable {
	public:
		void run()
		{
			std::lock_guard<std::mutex> lock(s_mutex);
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
				std::lock_guard<std::mutex> lock(s_mutex);
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

