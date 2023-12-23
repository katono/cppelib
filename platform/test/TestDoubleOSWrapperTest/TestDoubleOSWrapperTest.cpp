#include "TestDoubleOSWrapper/TestDoubleOSWrapper.h"
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
using OSWrapper::Timeout;

namespace {

class TestRunnable : public OSWrapper::Runnable {
public:
	void run()
	{
	}
};

}

TEST_GROUP(TestDoubleOSWrapperTest) {
	void setup()
	{
		TestDoubleOSWrapper::init();
	}
	void teardown()
	{
		mock().checkExpectations();
		mock().clear();
	}
};

TEST(TestDoubleOSWrapperTest, test_thread)
{
	TestRunnable testRun;
	Thread* thread = Thread::create(&testRun);
	CHECK(thread);

	thread->start();
	OSWrapper::Error err;
	err = thread->wait();
	LONGS_EQUAL(OSWrapper::OK, err);
	err = thread->tryWait();
	LONGS_EQUAL(OSWrapper::OK, err);
	err = thread->timedWait(Timeout::POLLING);
	LONGS_EQUAL(OSWrapper::OK, err);
	bool finished = thread->isFinished();
	CHECK_FALSE(finished);
	thread->setName("hoge");
	STRCMP_EQUAL("hoge", thread->getName());
	thread->setPriority(1);
	LONGS_EQUAL(1, thread->getPriority());
	LONGS_EQUAL(Thread::INHERIT_PRIORITY, thread->getInitialPriority());
	LONGS_EQUAL(0, thread->getStackSize());
	POINTERS_EQUAL(nullptr, thread->getNativeHandle());

	Thread::sleep(100);
	Thread::yield();
	POINTERS_EQUAL(nullptr, Thread::getCurrentThread());
	LONGS_EQUAL(0, Thread::getMaxPriority());
	LONGS_EQUAL(0, Thread::getMinPriority());
	LONGS_EQUAL(0, Thread::getHighestPriority());
	LONGS_EQUAL(0, Thread::getLowestPriority());

	Thread::destroy(thread);
}

TEST(TestDoubleOSWrapperTest, test_mutex)
{
	Mutex* mutex = Mutex::create();
	CHECK(mutex);
	{
		LockGuard lock(mutex);
	}
	OSWrapper::Error err;
	err = mutex->lock();
	LONGS_EQUAL(OSWrapper::OK, err);
	err = mutex->tryLock();
	LONGS_EQUAL(OSWrapper::OK, err);
	err = mutex->timedLock(Timeout::POLLING);
	LONGS_EQUAL(OSWrapper::OK, err);
	err = mutex->unlock();
	LONGS_EQUAL(OSWrapper::OK, err);
	Mutex::destroy(mutex);

	mutex = Mutex::create(1);
	Mutex::destroy(mutex);
}

TEST(TestDoubleOSWrapperTest, test_event_flag)
{
	EventFlag* ef = EventFlag::create(true);
	CHECK(ef);

	OSWrapper::Error err;
	err = ef->waitAny();
	LONGS_EQUAL(OSWrapper::OK, err);
	err = ef->waitOne(0);
	LONGS_EQUAL(OSWrapper::OK, err);
	err = ef->wait(EventFlag::Pattern(1), EventFlag::OR, nullptr);
	LONGS_EQUAL(OSWrapper::OK, err);

	err = ef->tryWaitAny();
	LONGS_EQUAL(OSWrapper::OK, err);
	err = ef->tryWaitOne(0);
	LONGS_EQUAL(OSWrapper::OK, err);
	err = ef->tryWait(EventFlag::Pattern(1), EventFlag::OR, nullptr);
	LONGS_EQUAL(OSWrapper::OK, err);

	err = ef->timedWaitAny(Timeout::POLLING);
	LONGS_EQUAL(OSWrapper::OK, err);
	err = ef->timedWaitOne(0, Timeout::POLLING);
	LONGS_EQUAL(OSWrapper::OK, err);
	err = ef->timedWait(EventFlag::Pattern(1), EventFlag::OR, nullptr, Timeout::POLLING);
	LONGS_EQUAL(OSWrapper::OK, err);

	err = ef->setAll();
	LONGS_EQUAL(OSWrapper::OK, err);
	err = ef->setOne(0);
	LONGS_EQUAL(OSWrapper::OK, err);
	err = ef->set(EventFlag::Pattern(1));
	LONGS_EQUAL(OSWrapper::OK, err);

	err = ef->resetAll();
	LONGS_EQUAL(OSWrapper::OK, err);
	err = ef->resetOne(0);
	LONGS_EQUAL(OSWrapper::OK, err);
	err = ef->reset(EventFlag::Pattern(1));
	LONGS_EQUAL(OSWrapper::OK, err);

	LONGS_EQUAL(EventFlag::Pattern(), ef->getCurrentPattern());

	EventFlag::destroy(ef);
}

TEST(TestDoubleOSWrapperTest, test_fixed_memory_pool)
{
	double poolBuf[100];
	FixedMemoryPool* pool = FixedMemoryPool::create(16, sizeof poolBuf, poolBuf);
	CHECK(pool);

	void* p = pool->allocate();
	CHECK(p);
	pool->deallocate(p);
	LONGS_EQUAL(16, pool->getBlockSize());
	LONGS_EQUAL(1600, FixedMemoryPool::getRequiredMemorySize(16, 100));

	FixedMemoryPool::destroy(pool);
}

TEST(TestDoubleOSWrapperTest, test_variable_memory_pool)
{
	double poolBuf[100];
	VariableMemoryPool* pool = VariableMemoryPool::create(sizeof poolBuf, poolBuf);
	CHECK(pool);

	void* p = pool->allocate(10);
	CHECK(p);
	pool->deallocate(p);

	VariableMemoryPool::destroy(pool);
}

TEST(TestDoubleOSWrapperTest, test_periodic_timer)
{
	TestRunnable testRun;
	PeriodicTimer* timer = PeriodicTimer::create(&testRun, 100);
	CHECK(timer);

	timer->start();
	timer->stop();
	CHECK_FALSE(timer->isStarted());
	LONGS_EQUAL(100, timer->getPeriodInMillis());
	timer->setName("hoge");
	STRCMP_EQUAL("hoge", timer->getName());

	PeriodicTimer::destroy(timer);
}

TEST(TestDoubleOSWrapperTest, test_oneshot_timer)
{
	TestRunnable testRun;
	OneShotTimer* timer = OneShotTimer::create(&testRun);
	CHECK(timer);

	timer->start(100);
	timer->stop();
	CHECK_FALSE(timer->isStarted());
	timer->setName("hoge");
	STRCMP_EQUAL("hoge", timer->getName());

	OneShotTimer::destroy(timer);
}
