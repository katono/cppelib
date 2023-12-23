#include "TestDoubleOSWrapper/TestDoubleOSWrapper.h"
#include "TestDoubleOSWrapper/TestDoubleThreadFactory.h"
#include "TestDoubleOSWrapper/TestDoubleMutexFactory.h"
#include "TestDoubleOSWrapper/TestDoubleEventFlagFactory.h"
#include "TestDoubleOSWrapper/TestDoubleFixedMemoryPoolFactory.h"
#include "TestDoubleOSWrapper/TestDoubleVariableMemoryPoolFactory.h"
#include "TestDoubleOSWrapper/TestDoublePeriodicTimerFactory.h"
#include "TestDoubleOSWrapper/TestDoubleOneShotTimerFactory.h"
#include "OSWrapper/Runnable.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/Mutex.h"
#include "OSWrapper/EventFlag.h"
#include "OSWrapper/FixedMemoryPool.h"
#include "OSWrapper/VariableMemoryPool.h"
#include "OSWrapper/PeriodicTimer.h"
#include "OSWrapper/OneShotTimer.h"

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

namespace td = TestDoubleOSWrapper;
using OSWrapper::Thread;
using OSWrapper::Mutex;
using OSWrapper::LockGuard;
using OSWrapper::EventFlag;
using OSWrapper::FixedMemoryPool;
using OSWrapper::VariableMemoryPool;
using OSWrapper::PeriodicTimer;
using OSWrapper::OneShotTimer;

namespace {

class MockThread : public td::TestDoubleThread {
public:
	virtual void start()
	{
		m_runnable->run();
	}
};

class MockThreadFactory : public td::TestDoubleThreadFactory<MockThread> {
public:
	virtual void sleep(unsigned long millis)
	{
		mock().actualCall("sleep").withParameter("millis", millis);
	}
};

class MockMutex : public td::TestDoubleMutex {
public:
	virtual OSWrapper::Error lock()
	{
		return (OSWrapper::Error) mock().actualCall("lock").onObject(this).returnIntValueOrDefault(OSWrapper::OK);
	}

	virtual OSWrapper::Error unlock()
	{
		return (OSWrapper::Error) mock().actualCall("unlock").onObject(this).returnIntValueOrDefault(OSWrapper::OK);
	}
};

class MockEventFlag : public td::TestDoubleEventFlag {
public:
	virtual OSWrapper::Error tryWaitAny()
	{
		return (OSWrapper::Error) mock().actualCall("tryWaitAny")
			.onObject(this).returnIntValueOrDefault(OSWrapper::OK);
	}
};

class MockFixedMemoryPool : public td::TestDoubleFixedMemoryPool {
public:
	virtual void* allocate()
	{
		// Allocation failed
		return 0;
	}
};

class MockVariableMemoryPool : public td::TestDoubleVariableMemoryPool {
public:
	virtual void* allocate(std::size_t size)
	{
		(void) size;
		// Allocation failed
		return 0;
	}
};

class MockPeriodicTimer : public td::TestDoublePeriodicTimer {
private:
	bool m_isStarted;
public:
	MockPeriodicTimer() : m_isStarted(false) {}

	virtual void start()
	{
		m_isStarted = true;
	}

	virtual void stop()
	{
		m_isStarted = false;
	}

	virtual bool isStarted() const
	{
		return m_isStarted;
	}
};

class MockOneShotTimer : public td::TestDoubleOneShotTimer {
public:
	virtual void start(unsigned long timeInMillis)
	{
		(void) timeInMillis;
		m_runnable->run();
	}
};

class TestRunnable : public OSWrapper::Runnable {
public:
	void run()
	{
		mock().actualCall("run").onObject(this);
	}
};

}

TEST_GROUP(OverrideTestDoubleOSWrapperTest) {
	void setup()
	{
		TestDoubleOSWrapper::init();
		// override TestDoubleThreadFactory & TestDoubleThread
		static MockThreadFactory theThreadFactory;
		OSWrapper::registerThreadFactory(&theThreadFactory);
		// override TestDoubleMutex
		static td::TestDoubleMutexFactory<MockMutex> theMutexFactory;
		OSWrapper::registerMutexFactory(&theMutexFactory);
		// override TestDoubleEventFlag
		static td::TestDoubleEventFlagFactory<MockEventFlag> theEventFlagFactory;
		OSWrapper::registerEventFlagFactory(&theEventFlagFactory);
		// override TestDoubleFixedMemoryPool
		static td::TestDoubleFixedMemoryPoolFactory<MockFixedMemoryPool> theFixedMemoryPoolFactory;
		OSWrapper::registerFixedMemoryPoolFactory(&theFixedMemoryPoolFactory);
		// override TestDoubleVariableMemoryPool
		static td::TestDoubleVariableMemoryPoolFactory<MockVariableMemoryPool> theVariableMemoryPoolFactory;
		OSWrapper::registerVariableMemoryPoolFactory(&theVariableMemoryPoolFactory);
		// override TestDoublePeriodicTimer
		static td::TestDoublePeriodicTimerFactory<MockPeriodicTimer> thePeriodicTimerFactory;
		OSWrapper::registerPeriodicTimerFactory(&thePeriodicTimerFactory);
		// override TestDoubleOneShotTimer
		static td::TestDoubleOneShotTimerFactory<MockOneShotTimer> theOneShotTimerFactory;
		OSWrapper::registerOneShotTimerFactory(&theOneShotTimerFactory);
	}
	void teardown()
	{
		mock().checkExpectations();
		mock().clear();
	}
};

TEST(OverrideTestDoubleOSWrapperTest, test_thread)
{
	mock().expectOneCall("sleep").withParameter("millis", 100);

	TestRunnable testRun;
	mock().expectOneCall("run").onObject(&testRun);
	Thread* thread = Thread::create(&testRun);
	CHECK(thread);

	thread->start();
	Thread::sleep(100);
	thread->wait();

	Thread::destroy(thread);
}

TEST(OverrideTestDoubleOSWrapperTest, test_mutex)
{
	Mutex* mutex = Mutex::create();
	CHECK(mutex);
	mock().expectOneCall("lock").onObject(mutex).andReturnValue(OSWrapper::OK);
	mock().expectOneCall("unlock").onObject(mutex).andReturnValue(OSWrapper::OK);

	{
		LockGuard lock(mutex);
	}

	Mutex::destroy(mutex);
}

TEST(OverrideTestDoubleOSWrapperTest, test_event_flag)
{
	EventFlag* ef = EventFlag::create(true);
	CHECK(ef);
	mock().expectOneCall("tryWaitAny").onObject(ef)
		.andReturnValue(OSWrapper::TimedOut);

	OSWrapper::Error err = ef->setAll();
	LONGS_EQUAL(OSWrapper::OK, err);
	err = ef->tryWaitAny();
	LONGS_EQUAL(OSWrapper::TimedOut, err);

	EventFlag::destroy(ef);
}

TEST(OverrideTestDoubleOSWrapperTest, test_fixed_memory_pool)
{
	double poolBuf[100];
	FixedMemoryPool* pool = FixedMemoryPool::create(16, sizeof poolBuf, poolBuf);
	CHECK(pool);

	void* p = pool->allocate();
	CHECK_FALSE(p);
	pool->deallocate(p);

	FixedMemoryPool::destroy(pool);
}

TEST(OverrideTestDoubleOSWrapperTest, test_variable_memory_pool)
{
	double poolBuf[100];
	VariableMemoryPool* pool = VariableMemoryPool::create(sizeof poolBuf, poolBuf);
	CHECK(pool);

	void* p = pool->allocate(10);
	CHECK_FALSE(p);
	pool->deallocate(p);

	VariableMemoryPool::destroy(pool);
}

TEST(OverrideTestDoubleOSWrapperTest, test_periodic_timer)
{
	TestRunnable testRun;
	PeriodicTimer* timer = PeriodicTimer::create(&testRun, 100);
	CHECK(timer);

	CHECK(!timer->isStarted());
	timer->start();
	CHECK(timer->isStarted());
	timer->stop();
	CHECK(!timer->isStarted());

	PeriodicTimer::destroy(timer);
}

TEST(OverrideTestDoubleOSWrapperTest, test_oneshot_timer)
{
	TestRunnable testRun;
	mock().expectOneCall("run").onObject(&testRun);
	OneShotTimer* timer = OneShotTimer::create(&testRun);
	CHECK(timer);

	timer->start(100);
	timer->stop();

	OneShotTimer::destroy(timer);
}
