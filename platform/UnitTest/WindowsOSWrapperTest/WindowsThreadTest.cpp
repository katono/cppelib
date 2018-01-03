#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "CppUTest/CommandLineTestRunner.h"
#include "OSWrapper/Thread.h"
#include "WindowsOSWrapper/WindowsThreadFactory.h"
#include "DesignByContract/Assertion.h"
#include <mutex>
#include <stdexcept>

using namespace OSWrapper;
using namespace WindowsOSWrapper;

static std::mutex s_mutex;

TEST_GROUP(WindowsThreadTest) {
	WindowsThreadFactory testFactory;
	Thread* thread;

	void setup()
	{
		Thread::setFactory(&testFactory);
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
	thread = Thread::create(&runnable, 4096, Thread::InheritPriority, "WindowsThread");
	CHECK(thread);
	Thread::destroy(thread);
}

TEST(WindowsThreadTest, start_join_isFinished)
{
	StaticMethodTestRunnable runnable;
	thread = Thread::create(&runnable, 4096, Thread::InheritPriority, "WindowsThread");
	runnable.setThread(thread);
	thread->start();

	CHECK(!thread->isFinished());

	thread->join();
	CHECK(thread->isFinished());

	Thread::destroy(thread);
}

TEST(WindowsThreadTest, create_destroy_no_runnable)
{
	thread = Thread::create(4096, Thread::InheritPriority, "TestThread");
	CHECK(thread);
	Thread::destroy(thread);
}

TEST(WindowsThreadTest, getCurrentThread_mainThread)
{
	Thread* mainThread = Thread::getCurrentThread();

	LONGS_EQUAL(1024*1024, mainThread->getStackSize());
	LONGS_EQUAL(4, mainThread->getPriority());
	STRCMP_EQUAL("MainThread", mainThread->getName());

	// Cannot destroy
	Thread::destroy(mainThread);
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
		t[i] = Thread::create(&runnable[i]);
	}

	for (int i = 0; i < num; i++) {
		t[i]->start();
	}

	for (int i = 0; i < num; i++) {
		t[i]->join();
	}

	for (int i = 0; i < num; i++) {
		Thread::destroy(t[i]);
	}
}

TEST(WindowsThreadTest, start_runnable)
{
	thread = Thread::create();
	MockRunnable runnable;
	mock().expectOneCall("run");
	thread->start(&runnable);
	Thread::destroy(thread);
}

TEST(WindowsThreadTest, repeat_start)
{
	MockRunnable runnable;
	thread = Thread::create(&runnable);
	mock().expectNCalls(2, "run");

	thread->start();
	thread->start();
	thread->join();

	thread->start(&runnable);
	thread->start(&runnable);
	thread->join();

	Thread::destroy(thread);
}

TEST(WindowsThreadTest, name)
{
	StaticMethodTestRunnable runnable;
	thread = Thread::create(&runnable, 4096, Thread::InheritPriority, "TestThread");

	STRCMP_EQUAL("TestThread", thread->getName());
	thread->setName("foo");
	STRCMP_EQUAL("foo", thread->getName());
	Thread::destroy(thread);
}

TEST(WindowsThreadTest, priority)
{
	StaticMethodTestRunnable runnable;
	thread = Thread::create(&runnable, 4096, 1, "TestThread");

	LONGS_EQUAL(1, thread->getPriority());
	const int prio = (Thread::getPriorityMax() + Thread::getPriorityMin()) / 2;
	thread->setPriority(prio);
	LONGS_EQUAL(prio, thread->getPriority());
	Thread::destroy(thread);
}

TEST(WindowsThreadTest, stackSize)
{
	StaticMethodTestRunnable runnable;
	thread = Thread::create(&runnable, 4096, Thread::InheritPriority, "TestThread");

	LONGS_EQUAL(4096, thread->getStackSize());
	Thread::destroy(thread);
}

class ThrowExceptionRunnable : public Runnable {
	bool m_isStdException;
public:
	ThrowExceptionRunnable(bool isStdException) : m_isStdException(isStdException) {}
	void run()
	{
		std::lock_guard<std::mutex> lock(s_mutex);
		if (m_isStdException) {
			throw std::runtime_error("Exception Test");
		} else {
			throw 1;
		}
	}
};

TEST(WindowsThreadTest, exception_std)
{
	ThrowExceptionRunnable runnable(true);
	thread = Thread::create(&runnable);
	thread->start();
	Thread::destroy(thread);
}

TEST(WindowsThreadTest, exception_unknown)
{
	ThrowExceptionRunnable runnable(false);
	thread = Thread::create(&runnable);
	thread->start();
	Thread::destroy(thread);
}

