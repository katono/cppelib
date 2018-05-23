#include "OSWrapper/Runnable.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/Mutex.h"
#include "ItronOSWrapper/ItronThreadFactory.h"
#include "ItronOSWrapper/ItronMutexFactory.h"
#include "Assertion/Assertion.h"
#include <stdexcept>
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

using OSWrapper::Runnable;
using OSWrapper::Thread;
using OSWrapper::Mutex;
using OSWrapper::LockGuard;
using ItronOSWrapper::ItronThreadFactory;
using ItronOSWrapper::ItronMutexFactory;

static Mutex* s_mutex;

TEST_GROUP(ItronThreadTest) {
	ItronThreadFactory testThreadFactory;
	ItronMutexFactory testMutexFactory;

	Thread* thread;

	void setup()
	{
		OSWrapper::registerMutexFactory(&testMutexFactory);
		OSWrapper::registerThreadFactory(&testThreadFactory);

		s_mutex = Mutex::create();
	}
	void teardown()
	{
		mock().checkExpectations();
		mock().clear();

		Mutex::destroy(s_mutex);
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


TEST(ItronThreadTest, create_destroy)
{
	StaticMethodTestRunnable runnable;
	thread = Thread::create(&runnable, Thread::getNormalPriority(), 4096, 0, "ItronThread");
	CHECK(thread);
	Thread::destroy(thread);
}

TEST(ItronThreadTest, start_wait_isFinished)
{
	StaticMethodTestRunnable runnable;
	thread = Thread::create(&runnable, Thread::getNormalPriority(), 4096, 0, "ItronThread");
	runnable.setThread(thread);
	thread->start();

	CHECK(!thread->isFinished());

	thread->wait();
	CHECK(thread->isFinished());

	Thread::destroy(thread);
}

TEST(ItronThreadTest, create_failed_runnable_nullptr)
{
	thread = Thread::create(0, Thread::getNormalPriority(), 4096, 0, "ItronThread");
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

TEST(ItronThreadTest, many_threads)
{
	const int num = 5;
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

TEST(ItronThreadTest, repeat_start)
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

TEST(ItronThreadTest, name)
{
	StaticMethodTestRunnable runnable;
	thread = Thread::create(&runnable, Thread::getNormalPriority(), 4096, 0, "TestThread");

	STRCMP_EQUAL("TestThread", thread->getName());
	thread->setName("foo");
	STRCMP_EQUAL("foo", thread->getName());
	Thread::destroy(thread);
}

TEST(ItronThreadTest, priority)
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

class NativeHandleTestRunnable : public Runnable {
public:
	void run()
	{
		LockGuard lock(s_mutex);
		Thread* t = Thread::getCurrentThread();
		ID tskid = (ID)t->getNativeHandle();
		T_RTSK rtsk = {0};
		ER err = ref_tsk(tskid, &rtsk);
		LONGS_EQUAL(E_OK, err);
		LONGS_EQUAL(TTS_RUN, rtsk.tskstat);
	}
};

TEST(ItronThreadTest, getNativeHandle)
{
	NativeHandleTestRunnable runnable;
	thread = Thread::create(&runnable, Thread::getNormalPriority());
	thread->start();

	thread->wait();
	Thread::destroy(thread);
}

TEST(ItronThreadTest, stackSize)
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

class MyExceptionHandler : public Thread::ExceptionHandler {
public:
	virtual void handle(Thread* t, const std::exception&)
	{
		mock().actualCall("handle").withParameter("t", t).onObject(this);
	}
};

TEST(ItronThreadTest, exception_std)
{
	ThrowExceptionRunnable runnable(ThrowExceptionRunnable::Std);
	thread = Thread::create(&runnable, Thread::getNormalPriority());

	MyExceptionHandler handler;
	thread->setExceptionHandler(&handler);
	mock().expectOneCall("handle").withParameter("t", thread).onObject(&handler);

	thread->start();
	Thread::destroy(thread);
}

TEST(ItronThreadTest, exception_assert)
{
	ThrowExceptionRunnable runnable(ThrowExceptionRunnable::Assert);
	thread = Thread::create(&runnable, Thread::getNormalPriority());

	MyExceptionHandler handler;
	thread->setExceptionHandler(&handler);
	mock().expectOneCall("handle").withParameter("t", thread).onObject(&handler);

	thread->start();
	Thread::destroy(thread);
}

TEST(ItronThreadTest, exception_unknown)
{
	ThrowExceptionRunnable runnable(ThrowExceptionRunnable::Integer);
	thread = Thread::create(&runnable, Thread::getNormalPriority());

	MyExceptionHandler handler;
	thread->setExceptionHandler(&handler);
	mock().expectOneCall("handle").withParameter("t", thread).onObject(&handler);

	thread->start();
	Thread::destroy(thread);
}

TEST(ItronThreadTest, setPriority_inherit)
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

