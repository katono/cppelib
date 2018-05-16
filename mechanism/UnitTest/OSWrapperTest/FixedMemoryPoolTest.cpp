#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "OSWrapper/FixedMemoryPool.h"
#include "OSWrapper/FixedMemoryPoolFactory.h"

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
};

TEST_GROUP(FixedMemoryPoolTest) {
	TestFixedMemoryPoolFactory testFactory;
	FixedMemoryPool* allocator;
	char pool[100];

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
	allocator = FixedMemoryPool::create(16, sizeof pool, pool);
	CHECK(allocator);
	FixedMemoryPool::destroy(allocator);
}

TEST(FixedMemoryPoolTest, create_no_param_pool)
{
	allocator = FixedMemoryPool::create(16, sizeof pool);
	CHECK(allocator);
	FixedMemoryPool::destroy(allocator);
}

TEST(FixedMemoryPoolTest, destroy_nullptr)
{
	FixedMemoryPool::destroy(0);
}

TEST(FixedMemoryPoolTest, allocate)
{
	allocator = FixedMemoryPool::create(16, sizeof pool, pool);
	mock().expectOneCall("allocate").onObject(allocator).andReturnValue(static_cast<void*>(pool));

	void* p = allocator->allocate();
	POINTERS_EQUAL(pool, p);

	FixedMemoryPool::destroy(allocator);
}

TEST(FixedMemoryPoolTest, allocate_failed)
{
	allocator = FixedMemoryPool::create(16, sizeof pool, pool);
	mock().expectOneCall("allocate").onObject(allocator);

	void* p = allocator->allocate();
	POINTERS_EQUAL(0, p);

	FixedMemoryPool::destroy(allocator);
}

TEST(FixedMemoryPoolTest, deallocate)
{
	allocator = FixedMemoryPool::create(16, sizeof pool, pool);
	void* p = pool;
	mock().expectOneCall("deallocate").onObject(allocator).withParameter("p", p);

	allocator->deallocate(p);

	FixedMemoryPool::destroy(allocator);
}

TEST(FixedMemoryPoolTest, deallocate_nullptr)
{
	allocator = FixedMemoryPool::create(16, sizeof pool, pool);
	void* p = 0;
	mock().expectOneCall("deallocate").onObject(allocator).withParameter("p", p);

	allocator->deallocate(p);

	FixedMemoryPool::destroy(allocator);
}

TEST(FixedMemoryPoolTest, getBlockSize)
{
	allocator = FixedMemoryPool::create(16, sizeof pool, pool);

	LONGS_EQUAL(16, allocator->getBlockSize());

	FixedMemoryPool::destroy(allocator);
}

