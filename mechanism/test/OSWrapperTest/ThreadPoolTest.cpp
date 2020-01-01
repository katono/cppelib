#include "OSWrapper/Runnable.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/ThreadFactory.h"
#include "OSWrapper/Mutex.h"
#include "OSWrapper/MutexFactory.h"
#include "OSWrapper/EventFlag.h"
#include "OSWrapper/EventFlagFactory.h"
#include "OSWrapper/FixedMemoryPool.h"
#include "OSWrapper/FixedMemoryPoolFactory.h"
#include "OSWrapper/MessageQueue.h"
#include "OSWrapper/ThreadPool.h"

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "CppUTest/MemoryLeakDetectorMallocMacros.h"

namespace ThreadPoolTest {

using OSWrapper::Runnable;
using OSWrapper::Thread;
using OSWrapper::ThreadFactory;
using OSWrapper::Mutex;
using OSWrapper::MutexFactory;
using OSWrapper::EventFlag;
using OSWrapper::EventFlagFactory;
using OSWrapper::FixedMemoryPool;
using OSWrapper::FixedMemoryPoolFactory;
using OSWrapper::MessageQueue;
using OSWrapper::ThreadPool;
using OSWrapper::Timeout;
using OSWrapper::LockGuard;

class TestThread : public Thread {
private:
	int m_priority;
	int m_initialPriority;
	std::size_t m_stackSize;
	void* m_stackAddress;
	const char* m_name;

public:
	TestThread(Runnable* r, int priority, std::size_t stackSize, void* stackAddress, const char* name)
	: Thread(r), m_priority(priority), m_initialPriority(priority), m_stackSize(stackSize), m_stackAddress(stackAddress), m_name(name)
	{
		if (stackSize == 0) {
			m_stackSize = 1024;
		}
		if (priority == Thread::INHERIT_PRIORITY) {
			m_priority = (Thread::getMaxPriority() + Thread::getMinPriority()) / 2;
			m_initialPriority = m_priority;
		}
	}

	~TestThread()
	{
	}

	void start()
	{
	}

	OSWrapper::Error wait()
	{
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
		return false;
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
	int getInitialPriority() const
	{
		return m_initialPriority;
	}
	std::size_t getStackSize() const
	{
		return m_stackSize;
	}
	void* getNativeHandle()
	{
		return (void*) 1234;
	}
};

class TestThreadFactory : public ThreadFactory {
public:
	int m_count;
	TestThreadFactory() : m_count(-1) {}
private:
	Thread* create(Runnable* r, int priority, std::size_t stackSize, void* stackAddress, const char* name)
	{
		if (m_count == 0) {
			return 0;
		}
		m_count--;
		Thread* t = new TestThread(r, priority, stackSize, stackAddress, name);
		return t;
	}
	void destroy(Thread* t)
	{
		delete static_cast<TestThread*>(t);
	}
	void sleep(unsigned long)
	{
	}
	void yield()
	{
	}
	Thread* getCurrentThread()
	{
		return 0;
	}
	int getMaxPriority() const
	{
		return 10;
	}
	int getMinPriority() const
	{
		return 1;
	}
	int getHighestPriority() const
	{
		return 10;
	}
	int getLowestPriority() const
	{
		return 1;
	}

};

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

TEST_GROUP(ThreadPoolTest) {
	TestThreadFactory testThreadFactory;
	TestMutexFactory testMutexFactory;
	TestEventFlagFactory testEventFlagFactory;
	TestFixedMemoryPoolFactory testFixedMemoryPoolFactory;

	void setup()
	{
		OSWrapper::registerThreadFactory(&testThreadFactory);
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

TEST(ThreadPoolTest, create_destroy)
{
	ThreadPool* tp = ThreadPool::create(10);
	CHECK(tp);
	ThreadPool::destroy(tp);
}

TEST(ThreadPoolTest, create_failed_objPool)
{
	testFixedMemoryPoolFactory.m_count = 0;
	ThreadPool* tp = ThreadPool::create(10);
	CHECK(tp == 0);

	testFixedMemoryPoolFactory.m_count = 1;
	testFixedMemoryPoolFactory.m_allocateCount = 0;
	tp = ThreadPool::create(10);
	CHECK(tp == 0);
}

TEST(ThreadPoolTest, create_failed_MessageQueue)
{
	testMutexFactory.m_count = 0;
	ThreadPool* tp = ThreadPool::create(10);
	CHECK(tp == 0);
}

TEST(ThreadPoolTest, create_failed_threadMemoryPool)
{
	testFixedMemoryPoolFactory.m_count = 2;
	ThreadPool* tp = ThreadPool::create(10);
	CHECK(tp == 0);

	testFixedMemoryPoolFactory.m_count = 3;
	testFixedMemoryPoolFactory.m_allocateCount = 2;
	tp = ThreadPool::create(10);
	CHECK(tp == 0);
}

TEST(ThreadPoolTest, create_failed_runnerMemoryPool)
{
	testFixedMemoryPoolFactory.m_count = 3;
	ThreadPool* tp = ThreadPool::create(10);
	CHECK(tp == 0);

	for (int i = 0; i < 10; i++) {
		testFixedMemoryPoolFactory.m_count = 4;
		testFixedMemoryPoolFactory.m_allocateCount = 3 + i;
		tp = ThreadPool::create(10);
		CHECK(tp == 0);
	}
}

TEST(ThreadPoolTest, create_failed_EventFlag)
{
	for (int i = 0; i < 10; i++) {
		testEventFlagFactory.m_count = 1 + i;
		ThreadPool* tp = ThreadPool::create(10);
		CHECK(tp == 0);
	}
}

TEST(ThreadPoolTest, create_failed_Thread)
{
	for (int i = 0; i < 10; i++) {
		testThreadFactory.m_count = i;
		ThreadPool* tp = ThreadPool::create(10);
		CHECK(tp == 0);
	}
}


} // namespace ThreadPoolTest
