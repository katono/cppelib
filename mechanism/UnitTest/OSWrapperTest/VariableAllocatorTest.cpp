#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "OSWrapper/VariableAllocator.h"
#include "OSWrapper/VariableAllocatorFactory.h"

using OSWrapper::VariableAllocator;
using OSWrapper::VariableAllocatorFactory;

class TestVariableAllocator : public VariableAllocator {
private:
	std::size_t m_memoryPoolSize;
	void* m_memoryPool;
public:
	TestVariableAllocator(std::size_t memoryPoolSize, void* memoryPool)
	: m_memoryPoolSize(memoryPoolSize), m_memoryPool(memoryPool) {}
	~TestVariableAllocator() {}

	void* allocate(std::size_t size)
	{
		return mock().actualCall("allocate").withParameter("size", size).onObject(this).returnPointerValueOrDefault(0);
	}
	void deallocate(void* p)
	{
		mock().actualCall("deallocate").withParameter("p", p).onObject(this);
	}
};

class TestVariableAllocatorFactory : public VariableAllocatorFactory {
public:
	VariableAllocator* create(std::size_t memoryPoolSize, void* memoryPool)
	{
		VariableAllocator* p = new TestVariableAllocator(memoryPoolSize, memoryPool);
		return p;
	}

	void destroy(VariableAllocator* p)
	{
		delete p;
	}
};

TEST_GROUP(VariableAllocatorTest) {
	TestVariableAllocatorFactory testFactory;
	VariableAllocator* allocator;
	char pool[100];

	void setup()
	{
		OSWrapper::registerVariableAllocatorFactory(&testFactory);
	}
	void teardown()
	{
		mock().checkExpectations();
		mock().clear();
	}
};

TEST(VariableAllocatorTest, create_destroy)
{
	allocator = VariableAllocator::create(sizeof pool, pool);
	CHECK(allocator);
	VariableAllocator::destroy(allocator);
}

TEST(VariableAllocatorTest, create_no_param_pool)
{
	allocator = VariableAllocator::create(sizeof pool);
	CHECK(allocator);
	VariableAllocator::destroy(allocator);
}

TEST(VariableAllocatorTest, destroy_nullptr)
{
	VariableAllocator::destroy(0);
}

TEST(VariableAllocatorTest, allocate)
{
	allocator = VariableAllocator::create(sizeof pool, pool);
	std::size_t size = 10;
	mock().expectOneCall("allocate").onObject(allocator).withParameter("size", size).andReturnValue(static_cast<void*>(pool));

	void* p = allocator->allocate(size);
	POINTERS_EQUAL(pool, p);

	VariableAllocator::destroy(allocator);
}

TEST(VariableAllocatorTest, allocate_failed)
{
	allocator = VariableAllocator::create(sizeof pool, pool);
	std::size_t size = 10;
	mock().expectOneCall("allocate").onObject(allocator).withParameter("size", size);

	void* p = allocator->allocate(size);
	POINTERS_EQUAL(0, p);

	VariableAllocator::destroy(allocator);
}

TEST(VariableAllocatorTest, deallocate)
{
	allocator = VariableAllocator::create(sizeof pool, pool);
	void* p = pool;
	mock().expectOneCall("deallocate").onObject(allocator).withParameter("p", p);

	allocator->deallocate(p);

	VariableAllocator::destroy(allocator);
}

TEST(VariableAllocatorTest, deallocate_nullptr)
{
	allocator = VariableAllocator::create(sizeof pool, pool);
	void* p = 0;
	mock().expectOneCall("deallocate").onObject(allocator).withParameter("p", p);

	allocator->deallocate(p);

	VariableAllocator::destroy(allocator);
}

