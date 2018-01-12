#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "CppUTest/CommandLineTestRunner.h"
#include "OSWrapper/Runnable.h"
#include "OSWrapper/Thread.h"
#include "WindowsOSWrapper/WindowsThreadFactory.h"
#include "OSWrapper/Mutex.h"
#include "WindowsOSWrapper/WindowsMutexFactory.h"
#include "DesignByContract/Assertion.h"
#include <stdexcept>
#include <windows.h>

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
		Thread::setFactory(&testThreadFactory);
		Mutex::setFactory(&testMutexFactory);
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

	LONGS_EQUAL(34, FibonacciTestRunnable::data);
}

class TryLockFailedTestRunnable : public Runnable {
	Timeout m_timeout;
public:
	TryLockFailedTestRunnable(Timeout tmout) : m_timeout(tmout) {}
	void run()
	{
		if (m_timeout == Timeout::POLLING) {
			Mutex::Error err = s_mutex->tryLock();
			LONGS_EQUAL(Mutex::TimedOut, err);
		} else {
			Mutex::Error err = s_mutex->tryLockFor(m_timeout);
			LONGS_EQUAL(Mutex::TimedOut, err);
		}
	}
};

TEST(WindowsMutexTest, tryLock)
{
	Mutex::Error err = s_mutex->tryLock();
	LONGS_EQUAL(Mutex::OK, err);

	TryLockFailedTestRunnable runnable(Timeout::POLLING);
	Thread* t = Thread::create(&runnable);
	t->start();
	t->join();
	Thread::destroy(t);

	s_mutex->unlock();
}

TEST(WindowsMutexTest, tryLockFor)
{
	Mutex::Error err = s_mutex->tryLockFor(Timeout(100));
	LONGS_EQUAL(Mutex::OK, err);

	TryLockFailedTestRunnable runnable(Timeout(10));
	Thread* t = Thread::create(&runnable);
	t->start();
	t->join();
	Thread::destroy(t);

	s_mutex->unlock();
}

TEST(WindowsMutexTest, tryLockFor_FOREVER)
{
	Mutex::Error err = s_mutex->tryLockFor(Timeout::FOREVER);
	LONGS_EQUAL(Mutex::OK, err);

	TryLockFailedTestRunnable runnable(Timeout::POLLING);
	Thread* t = Thread::create(&runnable);
	t->start();
	t->join();
	Thread::destroy(t);

	s_mutex->unlock();
}
