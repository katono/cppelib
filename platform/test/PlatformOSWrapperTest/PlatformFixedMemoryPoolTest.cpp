#include "OSWrapper/FixedMemoryPool.h"
#include "OSWrapper/Runnable.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/EventFlag.h"
#include "OSWrapper/MessageQueue.h"
#include <vector>
#include <cstring>
#include <cstdint>

#include "PlatformOSWrapperTestHelper.h"

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

namespace PlatformFixedMemoryPoolTest {

using OSWrapper::FixedMemoryPool;
using OSWrapper::FixedMemoryPoolFactory;
using OSWrapper::Timeout;
using OSWrapper::Runnable;
using OSWrapper::Thread;
using OSWrapper::EventFlag;
using OSWrapper::MessageQueue;

TEST_GROUP(PlatformFixedMemoryPoolTest) {
	void setup()
	{
		PlatformOSWrapperTestHelper::createAndRegisterOSWrapperFactories();
	}
	void teardown()
	{
		PlatformOSWrapperTestHelper::destroyOSWrapperFactories();

		mock().checkExpectations();
		mock().clear();
	}
};

TEST(PlatformFixedMemoryPoolTest, create_destroy)
{
	double poolBuf[100];
	FixedMemoryPool* pool = FixedMemoryPool::create(16, sizeof poolBuf, poolBuf);
	CHECK(pool);
	FixedMemoryPool::destroy(pool);
}

TEST(PlatformFixedMemoryPoolTest, create_no_param_pool)
{
	FixedMemoryPool* pool = FixedMemoryPool::create(16, 100);
	CHECK(pool);
	FixedMemoryPool::destroy(pool);
}

TEST(PlatformFixedMemoryPoolTest, create_failed_parameter)
{
	FixedMemoryPool* pool = FixedMemoryPool::create(0, 100);
	CHECK(!pool);

	pool = FixedMemoryPool::create(16, 0);
	CHECK(!pool);
}

TEST(PlatformFixedMemoryPoolTest, create_failed_buffer_too_small)
{
	double poolBuf[1];
	FixedMemoryPool* pool = FixedMemoryPool::create(16, sizeof poolBuf, poolBuf);
	CHECK(!pool);
}

TEST(PlatformFixedMemoryPoolTest, destroy_nullptr)
{
	FixedMemoryPool::destroy(0);
}

TEST(PlatformFixedMemoryPoolTest, getRequiredMemorySize)
{
	std::size_t size = FixedMemoryPool::getRequiredMemorySize(sizeof(int), 100);
	CHECK(size >= sizeof(int) * 100);
}

TEST(PlatformFixedMemoryPoolTest, allocate_deallocate)
{
	double poolBuf[100];
	FixedMemoryPool* pool = FixedMemoryPool::create(16, sizeof poolBuf, poolBuf);
	CHECK(pool);

	void* p = pool->allocate();
	CHECK(p);

	pool->deallocate(p);

	FixedMemoryPool::destroy(pool);
}

#if defined(PLATFORM_OS_ITRON)
TEST(PlatformFixedMemoryPoolTest, allocate_failed)
{
	const std::size_t blockSize = 16;
	FixedMemoryPool* pool = FixedMemoryPool::create(blockSize, FixedMemoryPool::getRequiredMemorySize(blockSize, 1));
	CHECK(pool);

	void* p = pool->allocate();
	CHECK(p);

	void* q = pool->allocate();
	CHECK(q == 0);

	pool->deallocate(p);

	q = pool->allocate();
	CHECK(q);
	pool->deallocate(q);

	FixedMemoryPool::destroy(pool);
}
#endif

TEST(PlatformFixedMemoryPoolTest, deallocate_nullptr)
{
	double poolBuf[100];
	FixedMemoryPool* pool = FixedMemoryPool::create(16, sizeof poolBuf, poolBuf);
	CHECK(pool);
	void* p = 0;

	pool->deallocate(p);

	FixedMemoryPool::destroy(pool);
}

TEST(PlatformFixedMemoryPoolTest, getBlockSize)
{
	double poolBuf[100];
	FixedMemoryPool* pool = FixedMemoryPool::create(16, sizeof poolBuf, poolBuf);
	CHECK(pool);

	LONGS_EQUAL(16, pool->getBlockSize());

	FixedMemoryPool::destroy(pool);
}

struct TestData {
	unsigned int a;
	unsigned short b;
	unsigned char c;
};

TEST(PlatformFixedMemoryPoolTest, allocateMemory)
{
	double poolBuf[100];
	FixedMemoryPool* pool = FixedMemoryPool::create(sizeof(TestData), sizeof poolBuf, poolBuf);
	CHECK(pool);

	void* p = 0;
	OSWrapper::Error err = pool->allocateMemory(&p);
	LONGS_EQUAL(OSWrapper::OK, err);
	CHECK(p);
	TestData* data = static_cast<TestData*>(p);
	data->a = 0xFFFFFFFF;
	data->b = 0xFFFF;
	data->c = 0xFF;

	pool->deallocate(p);

	FixedMemoryPool::destroy(pool);
}

TEST(PlatformFixedMemoryPoolTest, allocateMemory_using_os_prepared_memory)
{
	const std::size_t blockSize = sizeof(TestData);
	const std::size_t maxBlocks = 100;
	FixedMemoryPool* pool = FixedMemoryPool::create(blockSize, FixedMemoryPool::getRequiredMemorySize(blockSize, maxBlocks));
	CHECK(pool);

	LONGS_EQUAL(maxBlocks, pool->getMaxNumberOfBlocks());
	LONGS_EQUAL(maxBlocks, pool->getNumberOfAvailableBlocks());

	void* p = 0;
	OSWrapper::Error err = pool->allocateMemory(&p);
	LONGS_EQUAL(OSWrapper::OK, err);
	CHECK(p);
	TestData* data = static_cast<TestData*>(p);
	data->a = 0xFFFFFFFF;
	data->b = 0xFFFF;
	data->c = 0xFF;

	pool->deallocate(p);

	FixedMemoryPool::destroy(pool);
}

TEST(PlatformFixedMemoryPoolTest, allocateMemory_allocated_address_is_aligned)
{
	double poolBuf[100];
	FixedMemoryPool* pool = FixedMemoryPool::create(1, sizeof poolBuf, poolBuf);
	CHECK(pool);
	std::size_t maxBlocks = pool->getMaxNumberOfBlocks();
	for (std::size_t i = 0; i < maxBlocks; i++) {
		void* p = 0;
		OSWrapper::Error err = pool->allocateMemory(&p);
		LONGS_EQUAL(OSWrapper::OK, err);
		LONGS_EQUAL(0, (reinterpret_cast<std::uintptr_t>(p) & 0x07));
	}
	FixedMemoryPool::destroy(pool);

	FixedMemoryPool* pool2 = FixedMemoryPool::create(1, FixedMemoryPool::getRequiredMemorySize(1, 100));
	CHECK(pool2);
	std::size_t maxBlocks2 = pool2->getMaxNumberOfBlocks();
	LONGS_EQUAL(100, maxBlocks2);
	for (std::size_t i = 0; i < maxBlocks2; i++) {
		void* p = 0;
		OSWrapper::Error err = pool2->allocateMemory(&p);
		LONGS_EQUAL(OSWrapper::OK, err);
		LONGS_EQUAL(0, (reinterpret_cast<std::uintptr_t>(p) & 0x07));
	}
	FixedMemoryPool::destroy(pool2);
}

TEST(PlatformFixedMemoryPoolTest, allocateMemory_within_memory_range)
{
	double poolBuf[100];
	FixedMemoryPool* pool = FixedMemoryPool::create(16, sizeof poolBuf, poolBuf);
	CHECK(pool);
	std::size_t maxBlocks = pool->getMaxNumberOfBlocks();
	std::vector<const unsigned char*> ptrVec(maxBlocks);

	for (std::size_t i = 0; i < maxBlocks; i++) {
		void* p = 0;
		OSWrapper::Error err = pool->allocateMemory(&p);
		LONGS_EQUAL(OSWrapper::OK, err);
		CHECK_COMPARE(poolBuf, <=, p);
		CHECK_COMPARE(p, <, &poolBuf[100]);
		std::memset(p, static_cast<unsigned char>(i), 16);
		ptrVec[i] = static_cast<const unsigned char*>(p);
	}
	for (std::size_t i = 0; i < maxBlocks; i++) {
		const unsigned char* p = ptrVec[i];
		for (std::size_t j = 0; j < 16; j++) {
			LONGS_EQUAL(static_cast<unsigned char>(i), p[j]);
		}
	}

	FixedMemoryPool::destroy(pool);
}

TEST(PlatformFixedMemoryPoolTest, allocateMemory_invalid_parameter)
{
	double poolBuf[100];
	FixedMemoryPool* pool = FixedMemoryPool::create(16, sizeof poolBuf, poolBuf);
	CHECK(pool);

	OSWrapper::Error err = pool->allocateMemory(0);
	LONGS_EQUAL(OSWrapper::InvalidParameter, err);

	FixedMemoryPool::destroy(pool);
}

class AllocateRunnable : public Runnable {
private:
	FixedMemoryPool* m_pool;
	EventFlag* m_ev;
public:
	AllocateRunnable(FixedMemoryPool* pool, EventFlag* ev)
		: m_pool(pool), m_ev(ev)
	{}
	void run()
	{
		void* p = 0;
		OSWrapper::Error err = m_pool->allocateMemory(&p);
		LONGS_EQUAL(OSWrapper::OK, err);
		CHECK(p);
		m_ev->setOne(0);
		m_ev->waitOne(1);
		Thread::sleep(50);
		m_pool->deallocate(p);
	}
};

TEST(PlatformFixedMemoryPoolTest, allocateMemory_wait_for_available)
{
	const std::size_t blockSize = 16;
	const std::size_t maxBlocks = 100;
	FixedMemoryPool* pool = FixedMemoryPool::create(blockSize, FixedMemoryPool::getRequiredMemorySize(blockSize, maxBlocks));
	CHECK(pool);

	EventFlag* ev = EventFlag::create(true);
	CHECK(ev);

	AllocateRunnable runnable(pool, ev);
	Thread* thread = Thread::create(&runnable);
	CHECK(thread);
	thread->start();
	ev->waitOne(0);

	for (std::size_t i = 0; i < maxBlocks; i++) {
		if (pool->getNumberOfAvailableBlocks() == 0) {
			ev->setOne(1);
		}
		void* p = 0;
		OSWrapper::Error err = pool->allocateMemory(&p);
		LONGS_EQUAL(OSWrapper::OK, err);
		CHECK(p);
	}

	Thread::destroy(thread);
	EventFlag::destroy(ev);
	FixedMemoryPool::destroy(pool);
}

TEST(PlatformFixedMemoryPoolTest, allocateMemory_allocate_by_multi_threads)
{
	class SendRunnable : public Runnable {
	private:
		FixedMemoryPool* m_pool;
		MessageQueue<void*>* m_mq;
		const std::size_t m_num;
	public:
		SendRunnable(FixedMemoryPool* pool, MessageQueue<void*>* mq, std::size_t num)
			: m_pool(pool), m_mq(mq), m_num(num)
		{}
		void run()
		{
			for (std::size_t i = 0; i < m_num; i++) {
				void* p = 0;
				OSWrapper::Error err = m_pool->allocateMemory(&p);
				LONGS_EQUAL(OSWrapper::OK, err);
				CHECK(p);
				err = m_mq->send(p);
				LONGS_EQUAL(OSWrapper::OK, err);
			}
		}
	};
	class RecvRunnable : public Runnable {
	private:
		FixedMemoryPool* m_pool;
		MessageQueue<void*>* m_mq;
		EventFlag* m_ev;
	public:
		RecvRunnable(FixedMemoryPool* pool, MessageQueue<void*>* mq, EventFlag* ev)
			: m_pool(pool), m_mq(mq), m_ev(ev)
		{}
		void run()
		{
			while (true) {
				void* p = 0;
				OSWrapper::Error err = m_mq->timedReceive(&p, Timeout(10));
				if (err == OSWrapper::OK) {
					m_pool->deallocate(p);
				} else if (err == OSWrapper::TimedOut && m_ev->getCurrentPattern().any()) {
					break;
				}
			}
		}
	};

	const std::size_t blockSize = 16;
	const std::size_t maxBlocks = 100;
	FixedMemoryPool* pool = FixedMemoryPool::create(blockSize, FixedMemoryPool::getRequiredMemorySize(blockSize, maxBlocks));
	CHECK(pool);

	MessageQueue<void*>* mq = MessageQueue<void*>::create(maxBlocks);

	EventFlag* ev = EventFlag::create(true);
	CHECK(ev);

	SendRunnable sender(pool, mq, maxBlocks * 10);
	RecvRunnable receiver(pool, mq, ev);
	Thread* sendThread1 = Thread::create(&sender);
	Thread* sendThread2 = Thread::create(&sender);
	Thread* sendThread3 = Thread::create(&sender);
	Thread* recvThread = Thread::create(&receiver);
	CHECK(sendThread1);
	CHECK(sendThread2);
	CHECK(sendThread3);
	CHECK(recvThread);

	sendThread1->start();
	sendThread2->start();
	sendThread3->start();
	recvThread->start();

	sendThread1->wait();
	sendThread2->wait();
	sendThread3->wait();

	ev->setAll();
	recvThread->wait();

	// Check all blocks deallocated
	LONGS_EQUAL(0, mq->getSize());
	LONGS_EQUAL(maxBlocks, pool->getNumberOfAvailableBlocks());

	Thread::destroy(recvThread);
	Thread::destroy(sendThread1);
	Thread::destroy(sendThread2);
	Thread::destroy(sendThread3);
	EventFlag::destroy(ev);
	MessageQueue<void*>::destroy(mq);
	FixedMemoryPool::destroy(pool);
}

TEST(PlatformFixedMemoryPoolTest, tryAllocateMemory)
{
	double poolBuf[100];
	FixedMemoryPool* pool = FixedMemoryPool::create(sizeof(TestData), sizeof poolBuf, poolBuf);
	CHECK(pool);

	void* p = 0;
	OSWrapper::Error err = pool->tryAllocateMemory(&p);
	LONGS_EQUAL(OSWrapper::OK, err);
	CHECK(p);
	TestData* data = static_cast<TestData*>(p);
	data->a = 0xFFFFFFFF;
	data->b = 0xFFFF;
	data->c = 0xFF;

	pool->deallocate(p);

	FixedMemoryPool::destroy(pool);
}

TEST(PlatformFixedMemoryPoolTest, tryAllocateMemory_using_os_prepared_memory)
{
	const std::size_t blockSize = sizeof(TestData);
	const std::size_t maxBlocks = 100;
	FixedMemoryPool* pool = FixedMemoryPool::create(blockSize, FixedMemoryPool::getRequiredMemorySize(blockSize, maxBlocks));
	CHECK(pool);

	LONGS_EQUAL(maxBlocks, pool->getMaxNumberOfBlocks());
	LONGS_EQUAL(maxBlocks, pool->getNumberOfAvailableBlocks());

	void* p = 0;
	OSWrapper::Error err = pool->tryAllocateMemory(&p);
	LONGS_EQUAL(OSWrapper::OK, err);
	CHECK(p);
	TestData* data = static_cast<TestData*>(p);
	data->a = 0xFFFFFFFF;
	data->b = 0xFFFF;
	data->c = 0xFF;

	pool->deallocate(p);

	FixedMemoryPool::destroy(pool);
}

TEST(PlatformFixedMemoryPoolTest, tryAllocateMemory_allocated_address_is_aligned)
{
	double poolBuf[100];
	FixedMemoryPool* pool = FixedMemoryPool::create(1, sizeof poolBuf, poolBuf);
	CHECK(pool);
	std::size_t maxBlocks = pool->getMaxNumberOfBlocks();
	for (std::size_t i = 0; i < maxBlocks; i++) {
		void* p = 0;
		OSWrapper::Error err = pool->tryAllocateMemory(&p);
		LONGS_EQUAL(OSWrapper::OK, err);
		LONGS_EQUAL(0, (reinterpret_cast<std::uintptr_t>(p) & 0x07));
	}
	FixedMemoryPool::destroy(pool);

	FixedMemoryPool* pool2 = FixedMemoryPool::create(1, FixedMemoryPool::getRequiredMemorySize(1, 100));
	CHECK(pool2);
	std::size_t maxBlocks2 = pool2->getMaxNumberOfBlocks();
	LONGS_EQUAL(100, maxBlocks2);
	for (std::size_t i = 0; i < maxBlocks2; i++) {
		void* p = 0;
		OSWrapper::Error err = pool2->tryAllocateMemory(&p);
		LONGS_EQUAL(OSWrapper::OK, err);
		LONGS_EQUAL(0, (reinterpret_cast<std::uintptr_t>(p) & 0x07));
	}
	FixedMemoryPool::destroy(pool2);
}

TEST(PlatformFixedMemoryPoolTest, tryAllocateMemory_within_memory_range)
{
	double poolBuf[100];
	FixedMemoryPool* pool = FixedMemoryPool::create(16, sizeof poolBuf, poolBuf);
	CHECK(pool);
	std::size_t maxBlocks = pool->getMaxNumberOfBlocks();
	std::vector<const unsigned char*> ptrVec(maxBlocks);

	for (std::size_t i = 0; i < maxBlocks; i++) {
		void* p = 0;
		OSWrapper::Error err = pool->tryAllocateMemory(&p);
		LONGS_EQUAL(OSWrapper::OK, err);
		CHECK_COMPARE(poolBuf, <=, p);
		CHECK_COMPARE(p, <, &poolBuf[100]);
		std::memset(p, static_cast<unsigned char>(i), 16);
		ptrVec[i] = static_cast<const unsigned char*>(p);
	}
	for (std::size_t i = 0; i < maxBlocks; i++) {
		const unsigned char* p = ptrVec[i];
		for (std::size_t j = 0; j < 16; j++) {
			LONGS_EQUAL(static_cast<unsigned char>(i), p[j]);
		}
	}

	FixedMemoryPool::destroy(pool);
}

TEST(PlatformFixedMemoryPoolTest, tryAllocateMemory_invalid_parameter)
{
	double poolBuf[100];
	FixedMemoryPool* pool = FixedMemoryPool::create(16, sizeof poolBuf, poolBuf);
	CHECK(pool);

	OSWrapper::Error err = pool->tryAllocateMemory(0);
	LONGS_EQUAL(OSWrapper::InvalidParameter, err);

	FixedMemoryPool::destroy(pool);
}

TEST(PlatformFixedMemoryPoolTest, tryAllocateMemory_timeout_by_use_max)
{
	const std::size_t blockSize = 16;
	const std::size_t maxBlocks = 100;
	FixedMemoryPool* pool = FixedMemoryPool::create(blockSize, FixedMemoryPool::getRequiredMemorySize(blockSize, maxBlocks));
	CHECK(pool);

	LONGS_EQUAL(maxBlocks, pool->getMaxNumberOfBlocks());
	LONGS_EQUAL(maxBlocks, pool->getNumberOfAvailableBlocks());

	void* p = 0;
	for (std::size_t i = 0; i < maxBlocks; i++) {
		OSWrapper::Error err = pool->tryAllocateMemory(&p);
		LONGS_EQUAL(OSWrapper::OK, err);
		CHECK(p);
		LONGS_EQUAL(maxBlocks - 1 - i, pool->getNumberOfAvailableBlocks());
	}
	LONGS_EQUAL(0, pool->getNumberOfAvailableBlocks());

	void* q = 0;
	OSWrapper::Error err = pool->tryAllocateMemory(&q);
	LONGS_EQUAL(OSWrapper::TimedOut, err);

	pool->deallocate(p);
	LONGS_EQUAL(1, pool->getNumberOfAvailableBlocks());

	err = pool->tryAllocateMemory(&q);
	LONGS_EQUAL(OSWrapper::OK, err);
	CHECK(q);

	LONGS_EQUAL(0, pool->getNumberOfAvailableBlocks());
	err = pool->tryAllocateMemory(&q);
	LONGS_EQUAL(OSWrapper::TimedOut, err);

	FixedMemoryPool::destroy(pool);
}

TEST(PlatformFixedMemoryPoolTest, timedAllocateMemory)
{
	double poolBuf[100];
	FixedMemoryPool* pool = FixedMemoryPool::create(sizeof(TestData), sizeof poolBuf, poolBuf);
	CHECK(pool);

	void* p = 0;
	OSWrapper::Error err = pool->timedAllocateMemory(&p, Timeout(100));
	LONGS_EQUAL(OSWrapper::OK, err);
	CHECK(p);
	TestData* data = static_cast<TestData*>(p);
	data->a = 0xFFFFFFFF;
	data->b = 0xFFFF;
	data->c = 0xFF;

	pool->deallocate(p);

	FixedMemoryPool::destroy(pool);
}

TEST(PlatformFixedMemoryPoolTest, timedAllocateMemory_using_os_prepared_memory)
{
	const std::size_t blockSize = sizeof(TestData);
	const std::size_t maxBlocks = 100;
	FixedMemoryPool* pool = FixedMemoryPool::create(blockSize, FixedMemoryPool::getRequiredMemorySize(blockSize, maxBlocks));
	CHECK(pool);

	LONGS_EQUAL(maxBlocks, pool->getMaxNumberOfBlocks());
	LONGS_EQUAL(maxBlocks, pool->getNumberOfAvailableBlocks());

	void* p = 0;
	OSWrapper::Error err = pool->timedAllocateMemory(&p, Timeout(100));
	LONGS_EQUAL(OSWrapper::OK, err);
	CHECK(p);
	TestData* data = static_cast<TestData*>(p);
	data->a = 0xFFFFFFFF;
	data->b = 0xFFFF;
	data->c = 0xFF;

	pool->deallocate(p);

	FixedMemoryPool::destroy(pool);
}

TEST(PlatformFixedMemoryPoolTest, timedAllocateMemory_allocated_address_is_aligned)
{
	double poolBuf[100];
	FixedMemoryPool* pool = FixedMemoryPool::create(1, sizeof poolBuf, poolBuf);
	CHECK(pool);
	std::size_t maxBlocks = pool->getMaxNumberOfBlocks();
	for (std::size_t i = 0; i < maxBlocks; i++) {
		void* p = 0;
		OSWrapper::Error err = pool->timedAllocateMemory(&p, Timeout(100));
		LONGS_EQUAL(OSWrapper::OK, err);
		LONGS_EQUAL(0, (reinterpret_cast<std::uintptr_t>(p) & 0x07));
	}
	FixedMemoryPool::destroy(pool);

	FixedMemoryPool* pool2 = FixedMemoryPool::create(1, FixedMemoryPool::getRequiredMemorySize(1, 100));
	CHECK(pool2);
	std::size_t maxBlocks2 = pool2->getMaxNumberOfBlocks();
	LONGS_EQUAL(100, maxBlocks2);
	for (std::size_t i = 0; i < maxBlocks2; i++) {
		void* p = 0;
		OSWrapper::Error err = pool2->timedAllocateMemory(&p, Timeout(100));
		LONGS_EQUAL(OSWrapper::OK, err);
		LONGS_EQUAL(0, (reinterpret_cast<std::uintptr_t>(p) & 0x07));
	}
	FixedMemoryPool::destroy(pool2);
}

TEST(PlatformFixedMemoryPoolTest, timedAllocateMemory_within_memory_range)
{
	double poolBuf[100];
	FixedMemoryPool* pool = FixedMemoryPool::create(16, sizeof poolBuf, poolBuf);
	CHECK(pool);
	std::size_t maxBlocks = pool->getMaxNumberOfBlocks();
	std::vector<const unsigned char*> ptrVec(maxBlocks);

	for (std::size_t i = 0; i < maxBlocks; i++) {
		void* p = 0;
		OSWrapper::Error err = pool->timedAllocateMemory(&p, Timeout(100));
		LONGS_EQUAL(OSWrapper::OK, err);
		CHECK_COMPARE(poolBuf, <=, p);
		CHECK_COMPARE(p, <, &poolBuf[100]);
		std::memset(p, static_cast<unsigned char>(i), 16);
		ptrVec[i] = static_cast<const unsigned char*>(p);
	}
	for (std::size_t i = 0; i < maxBlocks; i++) {
		const unsigned char* p = ptrVec[i];
		for (std::size_t j = 0; j < 16; j++) {
			LONGS_EQUAL(static_cast<unsigned char>(i), p[j]);
		}
	}

	FixedMemoryPool::destroy(pool);
}

TEST(PlatformFixedMemoryPoolTest, timedAllocateMemory_invalid_parameter)
{
	double poolBuf[100];
	FixedMemoryPool* pool = FixedMemoryPool::create(16, sizeof poolBuf, poolBuf);
	CHECK(pool);

	OSWrapper::Error err = pool->timedAllocateMemory(0, Timeout(100));
	LONGS_EQUAL(OSWrapper::InvalidParameter, err);

	FixedMemoryPool::destroy(pool);
}

TEST(PlatformFixedMemoryPoolTest, timedAllocateMemory_wait_for_available)
{
	const std::size_t blockSize = 16;
	const std::size_t maxBlocks = 100;
	FixedMemoryPool* pool = FixedMemoryPool::create(blockSize, FixedMemoryPool::getRequiredMemorySize(blockSize, maxBlocks));
	CHECK(pool);

	EventFlag* ev = EventFlag::create(true);
	CHECK(ev);

	AllocateRunnable runnable(pool, ev);
	Thread* thread = Thread::create(&runnable);
	CHECK(thread);
	thread->start();
	ev->waitOne(0);

	for (std::size_t i = 0; i < maxBlocks; i++) {
		if (pool->getNumberOfAvailableBlocks() == 0) {
			ev->setOne(1);
		}
		void* p = 0;
		OSWrapper::Error err = pool->timedAllocateMemory(&p, Timeout(100));
		LONGS_EQUAL(OSWrapper::OK, err);
		CHECK(p);
	}

	Thread::destroy(thread);
	EventFlag::destroy(ev);
	FixedMemoryPool::destroy(pool);
}

TEST(PlatformFixedMemoryPoolTest, timedAllocateMemory_timeout_by_use_max)
{
	const std::size_t blockSize = 16;
	const std::size_t maxBlocks = 100;
	FixedMemoryPool* pool = FixedMemoryPool::create(blockSize, FixedMemoryPool::getRequiredMemorySize(blockSize, maxBlocks));
	CHECK(pool);

	LONGS_EQUAL(maxBlocks, pool->getMaxNumberOfBlocks());
	LONGS_EQUAL(maxBlocks, pool->getNumberOfAvailableBlocks());

	void* p = 0;
	for (std::size_t i = 0; i < maxBlocks; i++) {
		OSWrapper::Error err = pool->timedAllocateMemory(&p, Timeout(20));
		LONGS_EQUAL(OSWrapper::OK, err);
		CHECK(p);
		LONGS_EQUAL(maxBlocks - 1 - i, pool->getNumberOfAvailableBlocks());
	}
	LONGS_EQUAL(0, pool->getNumberOfAvailableBlocks());

	void* q = 0;
	OSWrapper::Error err = pool->timedAllocateMemory(&q, Timeout(20));
	LONGS_EQUAL(OSWrapper::TimedOut, err);

	pool->deallocate(p);
	LONGS_EQUAL(1, pool->getNumberOfAvailableBlocks());

	err = pool->timedAllocateMemory(&q, Timeout(20));
	LONGS_EQUAL(OSWrapper::OK, err);
	CHECK(q);

	LONGS_EQUAL(0, pool->getNumberOfAvailableBlocks());
	err = pool->timedAllocateMemory(&q, Timeout(20));
	LONGS_EQUAL(OSWrapper::TimedOut, err);

	FixedMemoryPool::destroy(pool);
}

} // namespace PlatformFixedMemoryPoolTest
