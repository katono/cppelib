#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "OSWrapper/Mutex.h"
#include "OSWrapper/MutexFactory.h"

using OSWrapper::Mutex;
using OSWrapper::MutexFactory;
using OSWrapper::Timeout;
using OSWrapper::LockGuard;

class TestMutex : public Mutex {
private:
	int m_priorityCeilingOrInherit;
public:
	TestMutex(int priorityCeilingOrInherit = -1)
	: m_priorityCeilingOrInherit(priorityCeilingOrInherit) {}
	~TestMutex() {}

	OSWrapper::Error lock()
	{
		return (OSWrapper::Error) mock().actualCall("lock").onObject(this).returnIntValueOrDefault(OSWrapper::OK);
	}

	OSWrapper::Error tryLock()
	{
		return (OSWrapper::Error) mock().actualCall("tryLock").onObject(this).returnIntValueOrDefault(OSWrapper::OK);
	}

	OSWrapper::Error timedLock(Timeout tmout)
	{
		return (OSWrapper::Error) mock().actualCall("timedLock").withParameter("tmout", tmout)
			.onObject(this).returnIntValueOrDefault(OSWrapper::OK);
	}

	OSWrapper::Error unlock()
	{
		return (OSWrapper::Error) mock().actualCall("unlock").onObject(this).returnIntValueOrDefault(OSWrapper::OK);
	}
};

class TestMutexFactory : public MutexFactory {
public:
	Mutex* create()
	{
		Mutex* m = new TestMutex();
		return m;
	}

	Mutex* create(int priorityCeiling)
	{
		Mutex* m = new TestMutex(priorityCeiling);
		return m;
	}

	void destroy(Mutex* m)
	{
		delete m;
	}
};

TEST_GROUP(MutexTest) {
	TestMutexFactory testFactory;
	Mutex* mutex;

	void setup()
	{
		OSWrapper::registerMutexFactory(&testFactory);
	}
	void teardown()
	{
		mock().checkExpectations();
		mock().clear();
	}
};

TEST(MutexTest, create_destroy)
{
	mutex = Mutex::create();
	CHECK(mutex);
	Mutex::destroy(mutex);
}

TEST(MutexTest, create_destroy_priorityCeiling)
{
	mutex = Mutex::create(10);
	CHECK(mutex);
	Mutex::destroy(mutex);
}

TEST(MutexTest, destroy_nullptr)
{
	Mutex::destroy(0);
}

TEST(MutexTest, lock)
{
	mutex = Mutex::create();
	mock().expectOneCall("lock").onObject(mutex).andReturnValue(OSWrapper::OK);

	OSWrapper::Error err = mutex->lock();
	LONGS_EQUAL(OSWrapper::OK, err);

	Mutex::destroy(mutex);
}

TEST(MutexTest, tryLock)
{
	mutex = Mutex::create();
	mock().expectOneCall("tryLock").onObject(mutex).andReturnValue(OSWrapper::OK);

	OSWrapper::Error err = mutex->tryLock();
	LONGS_EQUAL(OSWrapper::OK, err);

	Mutex::destroy(mutex);
}

TEST(MutexTest, timedLock)
{
	mutex = Mutex::create();
	mock().expectOneCall("timedLock").onObject(mutex).withParameter("tmout", Timeout(100)).andReturnValue(OSWrapper::TimedOut);

	OSWrapper::Error err = mutex->timedLock(Timeout(100));
	LONGS_EQUAL(OSWrapper::TimedOut, err);

	Mutex::destroy(mutex);
}

TEST(MutexTest, lock_recursive_err)
{
	mutex = Mutex::create();
	mock().expectOneCall("lock").onObject(mutex).andReturnValue(OSWrapper::OK);
	mock().expectOneCall("lock").onObject(mutex).andReturnValue(OSWrapper::LockedRecursively);

	OSWrapper::Error err = mutex->lock();
	LONGS_EQUAL(OSWrapper::OK, err);

	err = mutex->lock();
	LONGS_EQUAL(OSWrapper::LockedRecursively, err);

	Mutex::destroy(mutex);
}

TEST(MutexTest, unlock)
{
	mutex = Mutex::create();
	mock().expectOneCall("lock").onObject(mutex).andReturnValue(OSWrapper::OK);
	mock().expectOneCall("unlock").onObject(mutex).andReturnValue(OSWrapper::OK);

	OSWrapper::Error err = mutex->lock();
	LONGS_EQUAL(OSWrapper::OK, err);

	err = mutex->unlock();
	LONGS_EQUAL(OSWrapper::OK, err);

	Mutex::destroy(mutex);
}

TEST(MutexTest, unlock_not_locked)
{
	mutex = Mutex::create();
	mock().expectOneCall("unlock").onObject(mutex).andReturnValue(OSWrapper::NotLocked);

	OSWrapper::Error err = mutex->unlock();
	LONGS_EQUAL(OSWrapper::NotLocked, err);

	Mutex::destroy(mutex);
}

TEST(MutexTest, LockGuard)
{
	mutex = Mutex::create();
	mock().expectOneCall("lock").onObject(mutex).andReturnValue(OSWrapper::OK);
	mock().expectOneCall("unlock").onObject(mutex).andReturnValue(OSWrapper::OK);

	{
		LockGuard lock(mutex);
	}

	Mutex::destroy(mutex);
}

