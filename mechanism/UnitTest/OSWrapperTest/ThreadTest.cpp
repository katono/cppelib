#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "OSWrapper/Runnable.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/ThreadFactory.h"
#include "Assertion/Assertion.h"
#include <set>

using OSWrapper::Runnable;
using OSWrapper::Thread;
using OSWrapper::ThreadFactory;
using OSWrapper::Timeout;

class TestRunnable : public Runnable {
public:
	void run()
	{
		mock().actualCall("run").onObject(this);
	}
};

class TestThread : public Thread {
public:
	TestThread(Runnable* r, std::size_t stackSize, int priority, const char* name)
	: Thread(r, stackSize, priority, name)
	, m_finished(false)
	{
		if (stackSize == 0) {
			m_stackSize = 1024;
		}
		if (priority == Thread::INHERIT_PRIORITY) {
			m_priority = (Thread::getMaxPriority() + Thread::getMinPriority()) / 2;
		}
	}

	~TestThread()
	{
	}

	void start()
	{
		CHECK_PRECOND(m_runnable);
		m_runnable->run();
		m_finished = false;
	}

	OSWrapper::Error wait()
	{
		m_finished = true;
		return OSWrapper::OK;
	}

	OSWrapper::Error tryWait()
	{
		return OSWrapper::TimedOut;
	}

	OSWrapper::Error timedWait(Timeout)
	{
		return OSWrapper::TimedOut;
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
	void setPriority(int priority)
	{
		m_priority = priority;
	}
	int getPriority() const
	{
		return m_priority;
	}
	std::size_t getStackSize() const
	{
		return m_stackSize;
	}
	void* getNativeHandle()
	{
		return (void*) 1234;
	}


private:
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
	Thread* create(Runnable* r, std::size_t stackSize, int priority, const char* name)
	{
		Thread* t = new TestThread(r, stackSize, priority, name);
		m_threadSet.insert(t);
		return t;
	}
	void destroy(Thread* t)
	{
		std::size_t count = m_threadSet.erase(t);
		LONGS_EQUAL(1, count);
		delete static_cast<TestThread*>(t);
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
	int getMaxPriority() const
	{
		return 10;
	}
	int getMinPriority() const
	{
		return 0;
	}

};

TEST_GROUP(ThreadTest) {
	TestThreadFactory testFactory;
	TestRunnable testRun;
	Thread* thread;
	int normalPriority;

	void setup()
	{
		OSWrapper::registerThreadFactory(&testFactory);
		normalPriority = Thread::getNormalPriority();
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
	thread = Thread::create(&testRun, 4096, normalPriority, "TestThread");
	CHECK(thread);
	Thread::destroy(thread);
}

TEST(ThreadTest, create_failed_runnable_nullptr)
{
	thread = Thread::create(0, 4096, normalPriority, "TestThread");
	CHECK_FALSE(thread);
}

TEST(ThreadTest, destroy_nullptr)
{
	Thread::destroy(0);
}

TEST(ThreadTest, exit)
{
	try {
		Thread::exit();
	}
	catch (...) {
		return;
	}
	FAIL("failed");
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
	thread = Thread::create(&testRun, 4096, normalPriority, "TestThread");
	mock().expectOneCall("getCurrentThread").onObject(&testFactory).andReturnValue(thread);
	Thread* t = Thread::getCurrentThread();
	CHECK_EQUAL(thread, t);
	Thread::destroy(thread);
}

TEST(ThreadTest, start)
{
	thread = Thread::create(&testRun, 4096, normalPriority, "TestThread");
	mock().expectOneCall("run").onObject(&testRun);
	thread->start();
	Thread::destroy(thread);
}

TEST(ThreadTest, start_wait_isFinished)
{
	thread = Thread::create(&testRun, 4096, normalPriority, "TestThread");
	mock().expectOneCall("run").onObject(&testRun);
	thread->start();

	CHECK(!thread->isFinished());

	OSWrapper::Error err = thread->tryWait();
	LONGS_EQUAL(OSWrapper::TimedOut, err);

	err = thread->timedWait(Timeout(10));
	LONGS_EQUAL(OSWrapper::TimedOut, err);

	err = thread->wait();
	LONGS_EQUAL(OSWrapper::OK, err);

	CHECK(thread->isFinished());

	Thread::destroy(thread);
}

TEST(ThreadTest, name)
{
	thread = Thread::create(&testRun, 4096, normalPriority, "TestThread");

	STRCMP_EQUAL("TestThread", thread->getName());
	thread->setName("foo");
	STRCMP_EQUAL("foo", thread->getName());
	Thread::destroy(thread);
}

TEST(ThreadTest, priority)
{
	thread = Thread::create(&testRun, 4096, normalPriority, "TestThread");

	LONGS_EQUAL(normalPriority, thread->getPriority());
	thread->setPriority(normalPriority + 1);
	LONGS_EQUAL(normalPriority + 1, thread->getPriority());
	Thread::destroy(thread);
}

TEST(ThreadTest, stackSize)
{
	thread = Thread::create(&testRun, 4096, normalPriority, "TestThread");

	LONGS_EQUAL(4096, thread->getStackSize());
	Thread::destroy(thread);
}

TEST(ThreadTest, create_only_runnable)
{
	thread = Thread::create(&testRun);

	LONGS_EQUAL(1024, thread->getStackSize());
	LONGS_EQUAL(normalPriority, thread->getPriority());
	STRCMP_EQUAL("", thread->getName());

	Thread::destroy(thread);
}

TEST(ThreadTest, getNativeHandle)
{
	thread = Thread::create(&testRun);

	LONGS_EQUAL(1234, (int)thread->getNativeHandle());

	Thread::destroy(thread);
}

