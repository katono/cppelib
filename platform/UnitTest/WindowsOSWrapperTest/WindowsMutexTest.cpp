#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "OSWrapper/Runnable.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/Mutex.h"
#include "WindowsOSWrapper/WindowsThreadFactory.h"
#include "WindowsOSWrapper/WindowsMutexFactory.h"

using OSWrapper::Runnable;
using OSWrapper::Thread;
using OSWrapper::Mutex;
using OSWrapper::Timeout;
using OSWrapper::LockGuard;
using WindowsOSWrapper::WindowsThreadFactory;
using WindowsOSWrapper::WindowsMutexFactory;

static Mutex* s_mutex;

TEST_GROUP(WindowsMutexTest) {
	WindowsThreadFactory testThreadFactory;
	WindowsMutexFactory testMutexFactory;

	void setup()
	{
		OSWrapper::registerThreadFactory(&testThreadFactory);
		OSWrapper::registerMutexFactory(&testMutexFactory);
		s_mutex = Mutex::create();
	}
	void teardown()
	{
		mock().checkExpectations();
		mock().clear();
		Mutex::destroy(s_mutex);
	}
};

TEST(WindowsMutexTest, create_destroy)
{
	Mutex* mutex = Mutex::create();
	CHECK(mutex);
	Mutex::destroy(mutex);
}

TEST(WindowsMutexTest, create_destroy_priorityCeiling)
{
	Mutex* mutex = Mutex::create(10);
	CHECK(mutex);
	Mutex::destroy(mutex);
}

TEST(WindowsMutexTest, lock_unlock)
{
	Mutex* mutex = Mutex::create();

	mutex->lock();
	mutex->unlock();

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

TEST(WindowsMutexTest, shared_data_lock)
{
	FibonacciTestRunnable::init();
	const int num = 10;
	FibonacciTestRunnable runnable[num];

	Thread* t[num];
	for (int i = 0; i < num; i++) {
		t[i] = Thread::create(&runnable[i], 0, Thread::getNormalPriority());
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

TEST(WindowsMutexTest, shared_data_lock_LockGuard)
{
	FibonacciTestRunnable::init();
	const int num = 10;
	FibonacciTestRunnableLockGuard runnable[num];

	Thread* t[num];
	for (int i = 0; i < num; i++) {
		t[i] = Thread::create(&runnable[i], 0, Thread::getNormalPriority());
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

TEST(WindowsMutexTest, tryLock)
{
	OSWrapper::Error err = s_mutex->tryLock();
	LONGS_EQUAL(OSWrapper::OK, err);

	TryLockFailedTestRunnable runnable;
	Thread* t = Thread::create(&runnable, 0, Thread::getNormalPriority());
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

TEST(WindowsMutexTest, timedLock)
{
	OSWrapper::Error err = s_mutex->timedLock(Timeout(100));
	LONGS_EQUAL(OSWrapper::OK, err);

	TimedLockFailedTestRunnable runnable(Timeout(10));
	Thread* t = Thread::create(&runnable, 0, Thread::getNormalPriority());
	t->start();
	t->wait();
	Thread::destroy(t);

	s_mutex->unlock();
}

TEST(WindowsMutexTest, timedLock_FOREVER)
{
	OSWrapper::Error err = s_mutex->timedLock(Timeout::FOREVER);
	LONGS_EQUAL(OSWrapper::OK, err);

	TimedLockFailedTestRunnable runnable(Timeout::POLLING);
	Thread* t = Thread::create(&runnable, 0, Thread::getNormalPriority());
	t->start();
	t->wait();
	Thread::destroy(t);

	s_mutex->unlock();
}

TEST(WindowsMutexTest, unlock_not_locked)
{
	OSWrapper::Error err = s_mutex->unlock();
	LONGS_EQUAL(OSWrapper::NotLocked, err);
}

TEST(WindowsMutexTest, LockGuard_recursive)
{
	{
		LockGuard lock(s_mutex);
		{
			LockGuard lock2(s_mutex);
		}
	}
}

