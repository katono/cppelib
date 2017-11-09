#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/ThreadFactory.h"
#include "DesignByContract/Assertion.h"
#include <set>

using namespace OSWrapper;

class TestRunnable : public Runnable {
public:
	void run()
	{
		mock().actualCall("run").onObject(this);
	}
};

class TestThread : public Thread {
public:
	TestThread(Runnable* r, size_t stackSize, Thread::Priority prio, const char* name)
	: m_runnable(r), m_stackSize(stackSize), m_priority(prio), m_name(name)
	, m_finished(false)
	{
	}

	~TestThread()
	{
	}

	void start()
	{
		DBC_PRE(m_runnable);
		m_runnable->run();
		m_finished = false;
	}

	void start(Runnable* r)
	{
		m_runnable = r;
		start();
	}

	void join()
	{
		m_finished = true;
	}

	bool isFinished() const
	{
		return m_finished;
	}

	void setName(const char* name)
	{
		m_name = name;
	}
	const char* getName() const
	{
		return m_name;
	}
	void setPriority(Priority prio)
	{
		m_priority = prio;
	}
	Priority getPriority() const
	{
		return m_priority;
	}
	size_t getStackSize() const
	{
		return m_stackSize;
	}

private:
	Runnable* m_runnable;
	size_t m_stackSize;
	Priority m_priority;
	const char* m_name;

	bool m_finished;
};

class TestThreadFactory : public ThreadFactory {
private:
	std::set<Thread*> m_threadSet;
public:
	bool isMemoryLeaked() const
	{
		return !m_threadSet.empty();
	}
private:
	Thread* create(Runnable* r, size_t stackSize, Thread::Priority prio, const char* name)
	{
		Thread* t = new TestThread(r, stackSize, prio, name);
		m_threadSet.insert(t);
		return t;
	}
	void destroy(Thread* t)
	{
		m_threadSet.erase(t);
		delete t;
	}
	void exit()
	{
		mock().actualCall("exit").onObject(this);
	}
	void sleep(unsigned long millis)
	{
		mock().actualCall("sleep").withParameter("millis", millis).onObject(this);
	}
	void yield()
	{
		mock().actualCall("yield").onObject(this);
	}
	Thread* getCurrentThread()
	{
		return (Thread*) mock().actualCall("getCurrentThread").onObject(this).returnPointerValue();
	}
};

TEST_GROUP(ThreadTest) {
	TestThreadFactory testFactory;
	TestRunnable testRun;
	Thread* thread;

	void setup()
	{
		Thread::setFactory(&testFactory);
	}
	void teardown()
	{
		CHECK_FALSE(testFactory.isMemoryLeaked());

		mock().checkExpectations();
		mock().clear();
	}
};

TEST(ThreadTest, create_destroy)
{
	thread = Thread::create(&testRun, 4096, Thread::NormalPriority, "TestThread");
	CHECK(thread);
	Thread::destroy(thread);
}

TEST(ThreadTest, create_destroy_no_runnable)
{
	thread = Thread::create(4096, Thread::NormalPriority, "TestThread");
	CHECK(thread);
	Thread::destroy(thread);
}

TEST(ThreadTest, exit)
{
	mock().expectOneCall("exit").onObject(&testFactory);
	Thread::exit();
}

TEST(ThreadTest, sleep)
{
	mock().expectOneCall("sleep").withParameter("millis", 100).onObject(&testFactory);
	Thread::sleep(100);
}

TEST(ThreadTest, yield)
{
	mock().expectOneCall("yield").onObject(&testFactory);
	Thread::yield();
}

TEST(ThreadTest, getCurrentThread)
{
	thread = Thread::create(&testRun, 4096, Thread::NormalPriority, "TestThread");
	mock().expectOneCall("getCurrentThread").onObject(&testFactory).andReturnValue(thread);
	Thread* t = Thread::getCurrentThread();
	CHECK_EQUAL(thread, t);
	Thread::destroy(thread);
}

TEST(ThreadTest, start)
{
	thread = Thread::create(&testRun, 4096, Thread::NormalPriority, "TestThread");
	mock().expectOneCall("run").onObject(&testRun);
	thread->start();
	Thread::destroy(thread);
}

TEST(ThreadTest, start_exception)
{
	thread = Thread::create(4096, Thread::NormalPriority, "TestThread");
	try {
		thread->start();
	}
	catch (const std::exception& e) {
		STRCMP_CONTAINS("Pre-condition failed", e.what());
		Thread::destroy(thread);
		return;
	}
	FAIL("failed");
}

TEST(ThreadTest, start_runnable)
{
	thread = Thread::create(4096, Thread::NormalPriority, "TestThread");
	mock().expectOneCall("run").onObject(&testRun);
	thread->start(&testRun);
	Thread::destroy(thread);
}

TEST(ThreadTest, start_other_runnable)
{
	thread = Thread::create(&testRun, 4096, Thread::NormalPriority, "TestThread");
	TestRunnable r;
	mock().expectOneCall("run").onObject(&r);
	thread->start(&r);
	Thread::destroy(thread);
}

TEST(ThreadTest, start_join_isFinished)
{
	thread = Thread::create(&testRun, 4096, Thread::NormalPriority, "TestThread");
	mock().expectOneCall("run").onObject(&testRun);
	thread->start();

	CHECK(!thread->isFinished());

	thread->join();
	CHECK(thread->isFinished());

	Thread::destroy(thread);
}

TEST(ThreadTest, name)
{
	thread = Thread::create(&testRun, 4096, Thread::NormalPriority, "TestThread");

	STRCMP_EQUAL("TestThread", thread->getName());
	thread->setName("foo");
	STRCMP_EQUAL("foo", thread->getName());
	Thread::destroy(thread);
}

TEST(ThreadTest, priority)
{
	thread = Thread::create(&testRun, 4096, Thread::NormalPriority, "TestThread");

	LONGS_EQUAL(Thread::NormalPriority, thread->getPriority());
	thread->setPriority(Thread::LowPriority);
	LONGS_EQUAL(Thread::LowPriority, thread->getPriority());
	Thread::destroy(thread);
}

TEST(ThreadTest, stackSize)
{
	thread = Thread::create(&testRun, 4096, Thread::NormalPriority, "TestThread");

	LONGS_EQUAL(4096, thread->getStackSize());
	Thread::destroy(thread);
}

