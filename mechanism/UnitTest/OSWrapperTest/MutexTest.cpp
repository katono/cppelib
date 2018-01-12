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

	Mutex::Error lock()
	{
		return (Mutex::Error) mock().actualCall("lock").onObject(this).returnIntValueOrDefault(Mutex::OK);
	}

	Mutex::Error tryLock()
	{
		return (Mutex::Error) mock().actualCall("tryLock").onObject(this).returnIntValueOrDefault(Mutex::OK);
	}

	Mutex::Error tryLockFor(Timeout tmout)
	{
		return (Mutex::Error) mock().actualCall("tryLockFor").withParameter("tmout", tmout)
			.onObject(this).returnIntValueOrDefault(Mutex::OK);
	}

	Mutex::Error unlock()
	{
		return (Mutex::Error) mock().actualCall("unlock").onObject(this).returnIntValueOrDefault(Mutex::OK);
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
		Mutex::setFactory(&testFactory);
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
	mock().expectOneCall("lock").onObject(mutex).andReturnValue(Mutex::OK);

	Mutex::Error err = mutex->lock();
	LONGS_EQUAL(Mutex::OK, err);

	Mutex::destroy(mutex);
}

TEST(MutexTest, tryLock)
{
	mutex = Mutex::create();
	mock().expectOneCall("tryLock").onObject(mutex).andReturnValue(Mutex::OK);

	Mutex::Error err = mutex->tryLock();
	LONGS_EQUAL(Mutex::OK, err);

	Mutex::destroy(mutex);
}

TEST(MutexTest, tryLockFor)
{
	mutex = Mutex::create();
	mock().expectOneCall("tryLockFor").onObject(mutex).withParameter("tmout", Timeout(100)).andReturnValue(Mutex::TimedOut);

	Mutex::Error err = mutex->tryLockFor(Timeout(100));
	LONGS_EQUAL(Mutex::TimedOut, err);

	Mutex::destroy(mutex);
}

TEST(MutexTest, lock_recursive_err)
{
	mutex = Mutex::create();
	mock().expectOneCall("lock").onObject(mutex).andReturnValue(Mutex::OK);
	mock().expectOneCall("lock").onObject(mutex).andReturnValue(Mutex::LockedRecursively);

	Mutex::Error err = mutex->lock();
	LONGS_EQUAL(Mutex::OK, err);

	err = mutex->lock();
	LONGS_EQUAL(Mutex::LockedRecursively, err);

	Mutex::destroy(mutex);
}

TEST(MutexTest, unlock)
{
	mutex = Mutex::create();
	mock().expectOneCall("lock").onObject(mutex).andReturnValue(Mutex::OK);
	mock().expectOneCall("unlock").onObject(mutex).andReturnValue(Mutex::OK);

	Mutex::Error err = mutex->lock();
	LONGS_EQUAL(Mutex::OK, err);

	err = mutex->unlock();
	LONGS_EQUAL(Mutex::OK, err);

	Mutex::destroy(mutex);
}

TEST(MutexTest, unlock_not_locked)
{
	mutex = Mutex::create();
	mock().expectOneCall("unlock").onObject(mutex).andReturnValue(Mutex::NotLocked);

	Mutex::Error err = mutex->unlock();
	LONGS_EQUAL(Mutex::NotLocked, err);

	Mutex::destroy(mutex);
}

TEST(MutexTest, LockGuard)
{
	mutex = Mutex::create();
	mock().expectOneCall("lock").onObject(mutex).andReturnValue(Mutex::OK);
	mock().expectOneCall("unlock").onObject(mutex).andReturnValue(Mutex::OK);

	{
		LockGuard lock(mutex);
	}

	Mutex::destroy(mutex);
}

