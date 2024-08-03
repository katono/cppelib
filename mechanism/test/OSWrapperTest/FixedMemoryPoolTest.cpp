#include "OSWrapper/FixedMemoryPool.h"
#include "OSWrapper/FixedMemoryPoolFactory.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

namespace FixedMemoryPoolTest {

using OSWrapper::FixedMemoryPool;
using OSWrapper::FixedMemoryPoolFactory;

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

} // namespace FixedMemoryPoolTest
