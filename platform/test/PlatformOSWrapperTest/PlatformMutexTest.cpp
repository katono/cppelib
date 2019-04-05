#include "OSWrapper/Runnable.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/Mutex.h"

#ifdef PLATFORM_OS_WINDOWS
#include "WindowsOSWrapper/WindowsThreadFactory.h"
#include "WindowsOSWrapper/WindowsMutexFactory.h"
typedef WindowsOSWrapper::WindowsThreadFactory PlatformThreadFactory;
typedef WindowsOSWrapper::WindowsMutexFactory PlatformMutexFactory;
const int PRIORITY_CEILING = 10;
#elif PLATFORM_OS_STDCPP
#include "StdCppOSWrapper/StdCppThreadFactory.h"
#include "StdCppOSWrapper/StdCppMutexFactory.h"
typedef StdCppOSWrapper::StdCppThreadFactory PlatformThreadFactory;
typedef StdCppOSWrapper::StdCppMutexFactory PlatformMutexFactory;
const int PRIORITY_CEILING = 1;
#elif PLATFORM_OS_ITRON
#include "ItronOSWrapper/ItronThreadFactory.h"
#include "ItronOSWrapper/ItronMutexFactory.h"
typedef ItronOSWrapper::ItronThreadFactory PlatformThreadFactory;
typedef ItronOSWrapper::ItronMutexFactory PlatformMutexFactory;
const int PRIORITY_CEILING = 1;
#endif

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

namespace PlatformMutexTest {

using OSWrapper::Runnable;
using OSWrapper::Thread;
using OSWrapper::Mutex;
using OSWrapper::Timeout;
using OSWrapper::LockGuard;

static Mutex* s_mutex;

TEST_GROUP(PlatformMutexTest) {
	PlatformThreadFactory testThreadFactory;
	PlatformMutexFactory testMutexFactory;

	void setup()
	{
		OSWrapper::registerThreadFactory(&testThreadFactory);
		OSWrapper::registerMutexFactory(&testMutexFactory);

		s_mutex = Mutex::create();
	}
	void teardown()
	{
		OSWrapper::Error err = s_mutex->unlock();
		LONGS_EQUAL(OSWrapper::NotLocked, err);

		Mutex::destroy(s_mutex);

		mock().checkExpectations();
		mock().clear();
	}
};

TEST(PlatformMutexTest, create_destroy)
{
	Mutex* mutex = Mutex::create();
	CHECK(mutex);
	Mutex::destroy(mutex);
}

TEST(PlatformMutexTest, create_destroy_priorityCeiling)
{
	Mutex* mutex = Mutex::create(PRIORITY_CEILING);
	CHECK(mutex);
	Mutex::destroy(mutex);
}

TEST(PlatformMutexTest, lock_unlock)
{
	Mutex* mutex = Mutex::create();

	OSWrapper::Error err;
	err = mutex->lock();
	LONGS_EQUAL(OSWrapper::OK, err);
	err = mutex->unlock();
	LONGS_EQUAL(OSWrapper::OK, err);

	Mutex::destroy(mutex);
}

TEST(PlatformMutexTest, lock_unlock_recursive)
{
	Mutex* mutex = Mutex::create();

	OSWrapper::Error err;
	err = mutex->lock();
	LONGS_EQUAL(OSWrapper::OK, err);
	err = mutex->lock();
	LONGS_EQUAL(OSWrapper::OK, err);
	err = mutex->lock();
	LONGS_EQUAL(OSWrapper::OK, err);

	err = mutex->unlock();
	LONGS_EQUAL(OSWrapper::OK, err);
	err = mutex->unlock();
	LONGS_EQUAL(OSWrapper::OK, err);
	err = mutex->unlock();
	LONGS_EQUAL(OSWrapper::OK, err);

	err = mutex->unlock();
	LONGS_EQUAL(OSWrapper::NotLocked, err);

	Mutex::destroy(mutex);
}

class FibonacciTestRunnable : public Runnable {
public:
	static int data;
	static int count;

	static void init()
	{
		data = 0;
		count = 0;
	}

	virtual void run()
	{
		s_mutex->lock();
		fibonacci();
		s_mutex->unlock();
	}
protected:
	void fibonacci()
	{
		static int prev, pprev;
		Thread::yield();
		if (count == 0) {
			pprev = 0;
			prev = 0;
			data = 0;
		} else if (count == 1) {
			data = 1;
		} else {
			data = pprev + prev;
		}
		Thread::yield();
		pprev = prev;
		prev = data;
		count++;
	}
};
int FibonacciTestRunnable::data;
int FibonacciTestRunnable::count;

TEST(PlatformMutexTest, shared_data_lock)
{
	FibonacciTestRunnable::init();
	const int num = 10;
	FibonacciTestRunnable runnable[num];

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

	LONGS_EQUAL(34, FibonacciTestRunnable::data);
}

class FibonacciTestRunnableLockGuard : public FibonacciTestRunnable {
public:
	void run()
	{
		LockGuard lock(s_mutex);
		fibonacci();
	}
};

TEST(PlatformMutexTest, shared_data_lock_LockGuard)
{
	FibonacciTestRunnable::init();
	const int num = 10;
	FibonacciTestRunnableLockGuard runnable[num];

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

	LONGS_EQUAL(34, FibonacciTestRunnable::data);
}

class TryLockFailedTestRunnable : public Runnable {
public:
	void run()
	{
		OSWrapper::Error err = s_mutex->tryLock();
		LONGS_EQUAL(OSWrapper::TimedOut, err);
	}
};

TEST(PlatformMutexTest, tryLock)
{
	OSWrapper::Error err = s_mutex->tryLock();
	LONGS_EQUAL(OSWrapper::OK, err);

	TryLockFailedTestRunnable runnable;
	Thread* t = Thread::create(&runnable, Thread::getNormalPriority());
	t->start();
	t->wait();
	Thread::destroy(t);

	s_mutex->unlock();
}

class TimedLockFailedTestRunnable : public Runnable {
	Timeout m_timeout;
public:
	TimedLockFailedTestRunnable(Timeout tmout) : m_timeout(tmout) {}
	void run()
	{
		OSWrapper::Error err = s_mutex->timedLock(m_timeout);
		LONGS_EQUAL(OSWrapper::TimedOut, err);
	}
};

TEST(PlatformMutexTest, timedLock)
{
	OSWrapper::Error err = s_mutex->timedLock(Timeout(100));
	LONGS_EQUAL(OSWrapper::OK, err);

	TimedLockFailedTestRunnable runnable(Timeout(10));
	Thread* t = Thread::create(&runnable, Thread::getNormalPriority());
	t->start();
	t->wait();
	Thread::destroy(t);

	s_mutex->unlock();
}

TEST(PlatformMutexTest, timedLock_FOREVER)
{
	OSWrapper::Error err = s_mutex->timedLock(Timeout::FOREVER);
	LONGS_EQUAL(OSWrapper::OK, err);

	TimedLockFailedTestRunnable runnable(Timeout::POLLING);
	Thread* t = Thread::create(&runnable, Thread::getNormalPriority());
	t->start();
	t->wait();
	Thread::destroy(t);

	s_mutex->unlock();
}

TEST(PlatformMutexTest, unlock_not_locked)
{
	OSWrapper::Error err = s_mutex->unlock();
	LONGS_EQUAL(OSWrapper::NotLocked, err);
}

TEST(PlatformMutexTest, LockGuard_recursive)
{
	{
		LockGuard lock(s_mutex);
		{
			LockGuard lock2(s_mutex);
		}
	}
}

TEST(PlatformMutexTest, LockGuard_ADOPT_LOCK)
{
	{
		OSWrapper::Error err = s_mutex->tryLock();
		LONGS_EQUAL(OSWrapper::OK, err);

		LockGuard lock(s_mutex, LockGuard::ADOPT_LOCK);
	}
}

} // namespace PlatformMutexTest
