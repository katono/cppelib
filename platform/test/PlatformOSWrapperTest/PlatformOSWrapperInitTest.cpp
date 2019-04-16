#include "OSWrapper/Runnable.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/Mutex.h"
#include "OSWrapper/EventFlag.h"
#include "OSWrapper/MessageQueue.h"
#include "OSWrapper/FixedMemoryPool.h"
#include "OSWrapper/VariableMemoryPool.h"
#include "OSWrapper/PeriodicTimer.h"
#include "OSWrapper/OneShotTimer.h"
#include "Assertion/Assertion.h"
#include <stdexcept>

#ifdef PLATFORM_OS_WINDOWS
#include "WindowsOSWrapper/WindowsOSWrapper.h"
#elif PLATFORM_OS_POSIX
#include "PosixOSWrapper/PosixOSWrapper.h"
#elif PLATFORM_OS_STDCPP
#include "StdCppOSWrapper/StdCppOSWrapper.h"
#elif PLATFORM_OS_ITRON
#include "ItronOSWrapper/ItronOSWrapper.h"
#endif

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

namespace PlatformOSWrapperInitTest {

using OSWrapper::Runnable;
using OSWrapper::Thread;
using OSWrapper::Mutex;
using OSWrapper::LockGuard;
using OSWrapper::EventFlag;
using OSWrapper::MessageQueue;
using OSWrapper::FixedMemoryPool;
using OSWrapper::VariableMemoryPool;
using OSWrapper::PeriodicTimer;
using OSWrapper::OneShotTimer;
using OSWrapper::Timeout;


TEST_GROUP(PlatformOSWrapperInitTest) {
	void setup()
	{
		// No leak check because init() uses static objects.
		// Static objects are not released by teardown().
		MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();

#ifdef PLATFORM_OS_WINDOWS
		WindowsOSWrapper::init(1, 9);
#elif PLATFORM_OS_POSIX
		PosixOSWrapper::init(1, 9);
#elif PLATFORM_OS_STDCPP
		StdCppOSWrapper::init();
#elif PLATFORM_OS_ITRON
		ItronOSWrapper::init();
#endif

	}
	void teardown()
	{
		mock().checkExpectations();
		mock().clear();

		MemoryLeakWarningPlugin::turnOnNewDeleteOverloads();
	}
};

class TestRunnable : public Runnable {
public:
	void run()
	{
		Thread::sleep(100);
	}
};


TEST(PlatformOSWrapperInitTest, testThread)
{
	TestRunnable runnable;
	Thread* thread = Thread::create(&runnable);
	thread->start();

	CHECK(!thread->isFinished());

	thread->wait();

	CHECK(thread->isFinished());

	Thread::destroy(thread);
}

TEST(PlatformOSWrapperInitTest, testMutex)
{
	Mutex* mutex = Mutex::create();

	OSWrapper::Error err;
	err = mutex->lock();
	LONGS_EQUAL(OSWrapper::OK, err);
	err = mutex->unlock();
	LONGS_EQUAL(OSWrapper::OK, err);

	Mutex::destroy(mutex);
}

TEST(PlatformOSWrapperInitTest, testEventFlag)
{
	EventFlag* ef = EventFlag::create(true);

	OSWrapper::Error err = ef->setAll();
	LONGS_EQUAL(OSWrapper::OK, err);
	LONGS_EQUAL(EventFlag::Pattern().set(), ef->getCurrentPattern());

	EventFlag::destroy(ef);
}

TEST(PlatformOSWrapperInitTest, testMessageQueue)
{
	MessageQueue<int>* mq = MessageQueue<int>::create(10);

	OSWrapper::Error err = mq->send(100);
	LONGS_EQUAL(OSWrapper::OK, err);

	int data = 0;
	err = mq->receive(&data);
	LONGS_EQUAL(OSWrapper::OK, err);
	LONGS_EQUAL(100, data);

	MessageQueue<int>::destroy(mq);
}

TEST(PlatformOSWrapperInitTest, testFixedMemoryPool)
{
	FixedMemoryPool* pool = FixedMemoryPool::create(16, 1024);

	void* p = pool->allocate();
	CHECK(p);

	pool->deallocate(p);

	FixedMemoryPool::destroy(pool);
}

TEST(PlatformOSWrapperInitTest, testVariableMemoryPool)
{
	VariableMemoryPool* pool = VariableMemoryPool::create(1024);

	void* p = pool->allocate(10);
	CHECK(p);

	pool->deallocate(p);

	VariableMemoryPool::destroy(pool);
}

class TimerRunnable : public Runnable {
public:
	void run()
	{
	}
};

TEST(PlatformOSWrapperInitTest, testPeriodicTimer)
{
	TimerRunnable runnable;
	PeriodicTimer* timer = PeriodicTimer::create(&runnable, 100);

	timer->start();
	CHECK(timer->isStarted());

	timer->stop();

	PeriodicTimer::destroy(timer);
}

TEST(PlatformOSWrapperInitTest, testOneShotTimer)
{
	TimerRunnable runnable;
	OneShotTimer* timer = OneShotTimer::create(&runnable);

	timer->start(100);
	CHECK(timer->isStarted());

	timer->stop();

	OneShotTimer::destroy(timer);
}

} // namespace PlatformOSWrapperInitTest
