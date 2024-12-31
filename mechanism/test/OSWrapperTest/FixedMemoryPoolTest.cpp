#include "OSWrapper/FixedMemoryPool.h"
#include "OSWrapper/FixedMemoryPoolFactory.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

namespace FixedMemoryPoolTest {

using OSWrapper::FixedMemoryPool;
using OSWrapper::FixedMemoryPoolFactory;
using OSWrapper::Timeout;

class TestFixedMemoryPool : public FixedMemoryPool {
private:
	std::size_t m_blockSize;
	std::size_t m_memoryPoolSize;
	void* m_memoryPoolAddress;
public:
	TestFixedMemoryPool(std::size_t blockSize, std::size_t memoryPoolSize, void* memoryPoolAddress)
	: m_blockSize(blockSize), m_memoryPoolSize(memoryPoolSize), m_memoryPoolAddress(memoryPoolAddress) {}
	~TestFixedMemoryPool() {}

	void* allocate()
	{
		return mock().actualCall("allocate").onObject(this).returnPointerValueOrDefault(0);
	}
	void deallocate(void* p)
	{
		mock().actualCall("deallocate").withParameter("p", p).onObject(this);
	}
	std::size_t getBlockSize() const
	{
		return m_blockSize;
	}
};

class TestFixedMemoryPool2 : public TestFixedMemoryPool {
public:
	TestFixedMemoryPool2(std::size_t blockSize, std::size_t memoryPoolSize, void* memoryPoolAddress)
	: TestFixedMemoryPool(blockSize, memoryPoolSize, memoryPoolAddress) {}
	OSWrapper::Error allocateMemory(void** memory)
	{
		return (OSWrapper::Error) mock().actualCall("allocateMemory")
			.withOutputParameter("memory", memory)
			.onObject(this)
			.returnIntValueOrDefault(OSWrapper::OK);
	}
	OSWrapper::Error tryAllocateMemory(void** memory)
	{
		return (OSWrapper::Error) mock().actualCall("tryAllocateMemory")
			.withOutputParameter("memory", memory)
			.onObject(this)
			.returnIntValueOrDefault(OSWrapper::OK);
	}
	OSWrapper::Error timedAllocateMemory(void** memory, Timeout tmout)
	{
		return (OSWrapper::Error) mock().actualCall("timedAllocateMemory")
			.withOutputParameter("memory", memory)
			.withParameter("tmout", tmout)
			.onObject(this)
			.returnIntValueOrDefault(OSWrapper::OK);
	}
	std::size_t getNumberOfAvailableBlocks() const
	{
		return mock().actualCall("getNumberOfAvailableBlocks")
			.onObject(this)
			.returnIntValue();
	}

	std::size_t getMaxNumberOfBlocks() const
	{
		return mock().actualCall("getMaxNumberOfBlocks")
			.onObject(this)
			.returnIntValue();
	}
};

class TestFixedMemoryPoolFactory : public FixedMemoryPoolFactory {
public:
	FixedMemoryPool* create(std::size_t blockSize, std::size_t memoryPoolSize, void* memoryPoolAddress)
	{
		FixedMemoryPool* p = new TestFixedMemoryPool(blockSize, memoryPoolSize, memoryPoolAddress);
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

class TestFixedMemoryPoolFactory2 : public TestFixedMemoryPoolFactory {
public:
	FixedMemoryPool* create(std::size_t blockSize, std::size_t memoryPoolSize, void* memoryPoolAddress)
	{
		FixedMemoryPool* p = new TestFixedMemoryPool2(blockSize, memoryPoolSize, memoryPoolAddress);
		return p;
	}
	void destroy(FixedMemoryPool* p)
	{
		delete static_cast<TestFixedMemoryPool2*>(p);
	}
};

TEST_GROUP(FixedMemoryPoolTest) {
	TestFixedMemoryPoolFactory testFactory;

	void setup()
	{
		OSWrapper::registerFixedMemoryPoolFactory(&testFactory);
	}
	void teardown()
	{
		mock().checkExpectations();
		mock().clear();
	}
};

TEST(FixedMemoryPoolTest, create_destroy)
{
	double poolBuf[100];
	FixedMemoryPool* pool = FixedMemoryPool::create(16, sizeof poolBuf, poolBuf);
	CHECK(pool);
	FixedMemoryPool::destroy(pool);
}

TEST(FixedMemoryPoolTest, create_no_param_pool)
{
	FixedMemoryPool* pool = FixedMemoryPool::create(16, 100);
	CHECK(pool);
	FixedMemoryPool::destroy(pool);
}

TEST(FixedMemoryPoolTest, destroy_nullptr)
{
	FixedMemoryPool::destroy(0);
}

TEST(FixedMemoryPoolTest, getRequiredMemorySize)
{
	std::size_t size = FixedMemoryPool::getRequiredMemorySize(sizeof(int), 100);
	CHECK(size >= sizeof(int) * 100);
}

TEST(FixedMemoryPoolTest, allocate)
{
	double poolBuf[100];
	FixedMemoryPool* pool = FixedMemoryPool::create(16, sizeof poolBuf, poolBuf);
	CHECK(pool);
	mock().expectOneCall("allocate").onObject(pool).andReturnValue(static_cast<void*>(poolBuf));

	void* p = pool->allocate();
	POINTERS_EQUAL(poolBuf, p);

	FixedMemoryPool::destroy(pool);
}

TEST(FixedMemoryPoolTest, allocate_failed)
{
	double poolBuf[100];
	FixedMemoryPool* pool = FixedMemoryPool::create(16, sizeof poolBuf, poolBuf);
	CHECK(pool);
	mock().expectOneCall("allocate").onObject(pool);

	void* p = pool->allocate();
	POINTERS_EQUAL(0, p);

	FixedMemoryPool::destroy(pool);
}

TEST(FixedMemoryPoolTest, deallocate)
{
	double poolBuf[100];
	FixedMemoryPool* pool = FixedMemoryPool::create(16, sizeof poolBuf, poolBuf);
	CHECK(pool);
	void* p = poolBuf;
	mock().expectOneCall("deallocate").onObject(pool).withParameter("p", p);

	pool->deallocate(p);

	FixedMemoryPool::destroy(pool);
}

TEST(FixedMemoryPoolTest, deallocate_nullptr)
{
	double poolBuf[100];
	FixedMemoryPool* pool = FixedMemoryPool::create(16, sizeof poolBuf, poolBuf);
	CHECK(pool);
	void* p = 0;
	mock().expectOneCall("deallocate").onObject(pool).withParameter("p", p);

	pool->deallocate(p);

	FixedMemoryPool::destroy(pool);
}

TEST(FixedMemoryPoolTest, getBlockSize)
{
	double poolBuf[100];
	FixedMemoryPool* pool = FixedMemoryPool::create(16, sizeof poolBuf, poolBuf);
	CHECK(pool);

	LONGS_EQUAL(16, pool->getBlockSize());

	FixedMemoryPool::destroy(pool);
}

TEST(FixedMemoryPoolTest, allocateMemory_not_implemented)
{
	double poolBuf[100];
	FixedMemoryPool* pool = FixedMemoryPool::create(16, sizeof poolBuf, poolBuf);
	CHECK(pool);

	void* p;
	OSWrapper::Error err = pool->allocateMemory(&p);
	LONGS_EQUAL(OSWrapper::OtherError, err);

	err = pool->tryAllocateMemory(&p);
	LONGS_EQUAL(OSWrapper::OtherError, err);

	err = pool->timedAllocateMemory(&p, Timeout(100));
	LONGS_EQUAL(OSWrapper::OtherError, err);

	LONGS_EQUAL(0, pool->getNumberOfAvailableBlocks());
	LONGS_EQUAL(0, pool->getMaxNumberOfBlocks());

	FixedMemoryPool::destroy(pool);
}

struct TestData {
	unsigned int a;
	unsigned short b;
	unsigned char c;
};

TEST(FixedMemoryPoolTest, allocateMemory)
{
	TestFixedMemoryPoolFactory2 testFactory2;
	OSWrapper::registerFixedMemoryPoolFactory(&testFactory2);

	double poolBuf[100];
	FixedMemoryPool* pool = FixedMemoryPool::create(sizeof(TestData), sizeof poolBuf, poolBuf);
	CHECK(pool);
	void* memory = poolBuf;
	mock().expectOneCall("allocateMemory")
		.withOutputParameterReturning("memory", &memory, sizeof(memory))
		.onObject(pool)
		.andReturnValue(OSWrapper::OK);

	void* p = 0;
	OSWrapper::Error err = pool->allocateMemory(&p);
	LONGS_EQUAL(OSWrapper::OK, err);
	POINTERS_EQUAL(poolBuf, p);
	TestData* data = static_cast<TestData*>(p);
	data->a = 0xFFFFFFFF;
	data->b = 0xFFFF;
	data->c = 0xFF;

	FixedMemoryPool::destroy(pool);
}

TEST(FixedMemoryPoolTest, tryAllocateMemory)
{
	TestFixedMemoryPoolFactory2 testFactory2;
	OSWrapper::registerFixedMemoryPoolFactory(&testFactory2);

	double poolBuf[100];
	FixedMemoryPool* pool = FixedMemoryPool::create(sizeof(TestData), sizeof poolBuf, poolBuf);
	CHECK(pool);
	void* memory = poolBuf;
	mock().expectOneCall("tryAllocateMemory")
		.withOutputParameterReturning("memory", &memory, sizeof(memory))
		.onObject(pool)
		.andReturnValue(OSWrapper::OK);

	void* p = 0;
	OSWrapper::Error err = pool->tryAllocateMemory(&p);
	LONGS_EQUAL(OSWrapper::OK, err);
	POINTERS_EQUAL(poolBuf, p);
	TestData* data = static_cast<TestData*>(p);
	data->a = 0xFFFFFFFF;
	data->b = 0xFFFF;
	data->c = 0xFF;

	FixedMemoryPool::destroy(pool);
}

TEST(FixedMemoryPoolTest, timedAllocateMemory)
{
	TestFixedMemoryPoolFactory2 testFactory2;
	OSWrapper::registerFixedMemoryPoolFactory(&testFactory2);

	double poolBuf[100];
	FixedMemoryPool* pool = FixedMemoryPool::create(sizeof(TestData), sizeof poolBuf, poolBuf);
	CHECK(pool);
	void* memory = poolBuf;
	mock().expectOneCall("timedAllocateMemory")
		.withOutputParameterReturning("memory", &memory, sizeof(memory))
		.withParameter("tmout", Timeout(100))
		.onObject(pool)
		.andReturnValue(OSWrapper::OK);

	void* p = 0;
	OSWrapper::Error err = pool->timedAllocateMemory(&p, Timeout(100));
	LONGS_EQUAL(OSWrapper::OK, err);
	POINTERS_EQUAL(poolBuf, p);
	TestData* data = static_cast<TestData*>(p);
	data->a = 0xFFFFFFFF;
	data->b = 0xFFFF;
	data->c = 0xFF;

	FixedMemoryPool::destroy(pool);
}

TEST(FixedMemoryPoolTest, getNumberOfAvailableBlocks)
{
	TestFixedMemoryPoolFactory2 testFactory2;
	OSWrapper::registerFixedMemoryPoolFactory(&testFactory2);

	double poolBuf[100];
	FixedMemoryPool* pool = FixedMemoryPool::create(16, sizeof poolBuf, poolBuf);
	CHECK(pool);
	mock().expectOneCall("getNumberOfAvailableBlocks")
		.onObject(pool)
		.andReturnValue(50);

	LONGS_EQUAL(50, pool->getNumberOfAvailableBlocks());

	FixedMemoryPool::destroy(pool);
}

TEST(FixedMemoryPoolTest, getMaxNumberOfBlocks)
{
	TestFixedMemoryPoolFactory2 testFactory2;
	OSWrapper::registerFixedMemoryPoolFactory(&testFactory2);

	double poolBuf[100];
	FixedMemoryPool* pool = FixedMemoryPool::create(16, sizeof poolBuf, poolBuf);
	CHECK(pool);
	mock().expectOneCall("getMaxNumberOfBlocks")
		.onObject(pool)
		.andReturnValue(50);

	LONGS_EQUAL(50, pool->getMaxNumberOfBlocks());

	FixedMemoryPool::destroy(pool);
}

} // namespace FixedMemoryPoolTest
