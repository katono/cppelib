#include "OSWrapper/Mutex.h"
#include "OSWrapper/MutexFactory.h"
#include "OSWrapper/EventFlag.h"
#include "OSWrapper/EventFlagFactory.h"
#include "OSWrapper/FixedMemoryPool.h"
#include "OSWrapper/FixedMemoryPoolFactory.h"
#include "OSWrapper/MessageQueue.h"

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "CppUTest/MemoryLeakDetectorMallocMacros.h"

namespace MessageQueueTest {

using OSWrapper::Mutex;
using OSWrapper::MutexFactory;
using OSWrapper::EventFlag;
using OSWrapper::EventFlagFactory;
using OSWrapper::FixedMemoryPool;
using OSWrapper::FixedMemoryPoolFactory;
using OSWrapper::MessageQueue;
using OSWrapper::Timeout;
using OSWrapper::LockGuard;

class TestMutex : public Mutex {
public:
	TestMutex() {}
	~TestMutex() {}

	OSWrapper::Error lock()
	{
		return OSWrapper::OK;
	}

	OSWrapper::Error tryLock()
	{
		return OSWrapper::OK;
	}

	OSWrapper::Error timedLock(Timeout)
	{
		return OSWrapper::OK;
	}

	OSWrapper::Error unlock()
	{
		return OSWrapper::OK;
	}
};

class TestMutexFactory : public MutexFactory {
public:
	int m_count;
	TestMutexFactory() : m_count(-1) {}
	Mutex* create()
	{
		if (m_count == 0) {
			return 0;
		}
		m_count--;
		Mutex* m = new TestMutex();
		return m;
	}

	Mutex* create(int)
	{
		return create();
	}

	void destroy(Mutex* m)
	{
		delete static_cast<TestMutex*>(m);
	}
};

class TestEventFlag : public EventFlag {
	bool m_autoReset;
public:
	TestEventFlag(bool autoReset) : m_autoReset(autoReset) {}
	OSWrapper::Error waitAny()
	{
		return OSWrapper::OK;
	}
	OSWrapper::Error waitOne(std::size_t)
	{
		return OSWrapper::OK;
	}
	OSWrapper::Error wait(Pattern, Mode, Pattern*)
	{
		return OSWrapper::OK;
	}
	OSWrapper::Error tryWaitAny()
	{
		return OSWrapper::OK;
	}
	OSWrapper::Error tryWaitOne(std::size_t)
	{
		return OSWrapper::OK;
	}
	OSWrapper::Error tryWait(Pattern, Mode, Pattern*)
	{
		return OSWrapper::OK;
	}
	OSWrapper::Error timedWaitAny(Timeout)
	{
		return OSWrapper::TimedOut;
	}
	OSWrapper::Error timedWaitOne(std::size_t, Timeout)
	{
		return OSWrapper::TimedOut;
	}
	OSWrapper::Error timedWait(Pattern, Mode, Pattern*, Timeout)
	{
		return OSWrapper::TimedOut;
	}

	OSWrapper::Error setAll()
	{
		return OSWrapper::OK;
	}
	OSWrapper::Error setOne(std::size_t)
	{
		return OSWrapper::OK;
	}
	OSWrapper::Error set(Pattern)
	{
		return OSWrapper::OK;
	}

	OSWrapper::Error resetAll()
	{
		return OSWrapper::OK;
	}
	OSWrapper::Error resetOne(std::size_t)
	{
		return OSWrapper::OK;
	}
	OSWrapper::Error reset(Pattern)
	{
		return OSWrapper::OK;
	}

	Pattern getCurrentPattern() const
	{
		return 0;
	}
};

class TestEventFlagFactory : public EventFlagFactory {
public:
	int m_count;
	TestEventFlagFactory() : m_count(-1) {}
	EventFlag* create(bool autoReset)
	{
		if (m_count == 0) {
			return 0;
		}
		m_count--;
		EventFlag* e = new TestEventFlag(autoReset);
		return e;
	}

	void destroy(EventFlag* e)
	{
		delete static_cast<TestEventFlag*>(e);
	}
};

class TestFixedMemoryPool : public FixedMemoryPool {
private:
	std::size_t m_blockSize;
	int& m_count;
public:
	TestFixedMemoryPool(std::size_t blockSize, int& count)
	: m_blockSize(blockSize), m_count(count) {}
	~TestFixedMemoryPool() {}

	void* allocate()
	{
		if (m_count == 0) {
			return 0;
		}
		m_count--;
		return malloc(m_blockSize);
	}
	void deallocate(void* p)
	{
		free(p);
	}
	std::size_t getBlockSize() const
	{
		return m_blockSize;
	}
};

class TestFixedMemoryPoolFactory : public FixedMemoryPoolFactory {
public:
	int m_count;
	int m_allocateCount;
	TestFixedMemoryPoolFactory() : m_count(-1), m_allocateCount(-1) {}
	FixedMemoryPool* create(std::size_t blockSize, std::size_t, void*)
	{
		if (m_count == 0) {
			return 0;
		}
		m_count--;
		FixedMemoryPool* p = new TestFixedMemoryPool(blockSize, m_allocateCount);
		return p;
	}

	void destroy(FixedMemoryPool* p)
	{
		delete static_cast<TestFixedMemoryPool*>(p);
	}

	std::size_t getRequiredMemorySize(std::size_t blockSize, std::size_t numBlocks)
	{
		return blockSize * numBlocks;
	}
};

TEST_GROUP(MessageQueueTest) {
	TestMutexFactory testMutexFactory;
	TestEventFlagFactory testEventFlagFactory;
	TestFixedMemoryPoolFactory testFixedMemoryPoolFactory;
	typedef MessageQueue<int> IntMQ;

	void setup()
	{
		OSWrapper::registerMutexFactory(&testMutexFactory);
		OSWrapper::registerEventFlagFactory(&testEventFlagFactory);
		OSWrapper::registerFixedMemoryPoolFactory(&testFixedMemoryPoolFactory);
	}
	void teardown()
	{
		mock().checkExpectations();
		mock().clear();
	}
};

TEST(MessageQueueTest, create_destroy)
{
	IntMQ* mq = IntMQ::create(10);
	CHECK(mq);
	IntMQ::destroy(mq);
}

TEST(MessageQueueTest, create_failed_FixedMemoryPool)
{
	testFixedMemoryPoolFactory.m_count = 0;
	IntMQ* mq = IntMQ::create(10);
	CHECK(mq == 0);

	testFixedMemoryPoolFactory.m_count = 1;
	testFixedMemoryPoolFactory.m_allocateCount = 0;
	mq = IntMQ::create(10);
	CHECK(mq == 0);
}

TEST(MessageQueueTest, create_failed_Mutex)
{
	testMutexFactory.m_count = 0;
	IntMQ* mq = IntMQ::create(10);
	CHECK(mq == 0);

	testMutexFactory.m_count = 1;
	mq = IntMQ::create(10);
	CHECK(mq == 0);

	testMutexFactory.m_count = 2;
	mq = IntMQ::create(10);
	CHECK(mq == 0);
}

TEST(MessageQueueTest, create_failed_EventFlag)
{
	testEventFlagFactory.m_count = 0;
	IntMQ* mq = IntMQ::create(10);
	CHECK(mq == 0);
}


} // namespace MessageQueueTest
