#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "OSWrapper/FixedAllocator.h"
#include "OSWrapper/FixedAllocatorFactory.h"

using OSWrapper::FixedAllocator;
using OSWrapper::FixedAllocatorFactory;

class TestFixedAllocator : public FixedAllocator {
private:
	std::size_t m_blockSize;
	std::size_t m_memoryPoolSize;
	void* m_memoryPool;
public:
	TestFixedAllocator(std::size_t blockSize, std::size_t memoryPoolSize, void* memoryPool)
	: m_blockSize(blockSize), m_memoryPoolSize(memoryPoolSize), m_memoryPool(memoryPool) {}
	~TestFixedAllocator() {}

	void* allocate()
	{
		return mock().actualCall("allocate").onObject(this).returnPointerValueOrDefault(0);
	}
	void deallocate(void* p)
	{
		mock().actualCall("deallocate").withParameter("p", p).onObject(this);
	}
};

class TestFixedAllocatorFactory : public FixedAllocatorFactory {
public:
	FixedAllocator* create(std::size_t blockSize, std::size_t memoryPoolSize, void* memoryPool)
	{
		FixedAllocator* p = new TestFixedAllocator(blockSize, memoryPoolSize, memoryPool);
		return p;
	}

	void destroy(FixedAllocator* p)
	{
		delete p;
	}
};

TEST_GROUP(FixedAllocatorTest) {
	TestFixedAllocatorFactory testFactory;
	FixedAllocator* allocator;
	char pool[100];

	void setup()
	{
		OSWrapper::registerFixedAllocatorFactory(&testFactory);
	}
	void teardown()
	{
		mock().checkExpectations();
		mock().clear();
	}
};

TEST(FixedAllocatorTest, create_destroy)
{
	allocator = FixedAllocator::create(16, sizeof pool, pool);
	CHECK(allocator);
	FixedAllocator::destroy(allocator);
}

TEST(FixedAllocatorTest, create_no_param_pool)
{
	allocator = FixedAllocator::create(16, sizeof pool);
	CHECK(allocator);
	FixedAllocator::destroy(allocator);
}

TEST(FixedAllocatorTest, destroy_nullptr)
{
	FixedAllocator::destroy(0);
}

TEST(FixedAllocatorTest, allocate)
{
	allocator = FixedAllocator::create(16, sizeof pool, pool);
	mock().expectOneCall("allocate").onObject(allocator).andReturnValue(static_cast<void*>(pool));

	void* p = allocator->allocate();
	POINTERS_EQUAL(pool, p);

	FixedAllocator::destroy(allocator);
}

TEST(FixedAllocatorTest, allocate_failed)
{
	allocator = FixedAllocator::create(16, sizeof pool, pool);
	mock().expectOneCall("allocate").onObject(allocator);

	void* p = allocator->allocate();
	POINTERS_EQUAL(0, p);

	FixedAllocator::destroy(allocator);
}

TEST(FixedAllocatorTest, deallocate)
{
	allocator = FixedAllocator::create(16, sizeof pool, pool);
	void* p = pool;
	mock().expectOneCall("deallocate").onObject(allocator).withParameter("p", p);

	allocator->deallocate(p);

	FixedAllocator::destroy(allocator);
}

TEST(FixedAllocatorTest, deallocate_nullptr)
{
	allocator = FixedAllocator::create(16, sizeof pool, pool);
	void* p = 0;
	mock().expectOneCall("deallocate").onObject(allocator).withParameter("p", p);

	allocator->deallocate(p);

	FixedAllocator::destroy(allocator);
}

