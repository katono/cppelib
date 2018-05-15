#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "OSWrapper/VariableMemoryPool.h"
#include "OSWrapper/VariableMemoryPoolFactory.h"

using OSWrapper::VariableMemoryPool;
using OSWrapper::VariableMemoryPoolFactory;

class TestVariableMemoryPool : public VariableMemoryPool {
private:
	std::size_t m_memoryPoolSize;
	void* m_memoryPool;
public:
	TestVariableMemoryPool(std::size_t memoryPoolSize, void* memoryPool)
	: m_memoryPoolSize(memoryPoolSize), m_memoryPool(memoryPool) {}
	~TestVariableMemoryPool() {}

	void* allocate(std::size_t size)
	{
		return mock().actualCall("allocate").withParameter("size", size).onObject(this).returnPointerValueOrDefault(0);
	}
	void deallocate(void* p)
	{
		mock().actualCall("deallocate").withParameter("p", p).onObject(this);
	}
};

class TestVariableMemoryPoolFactory : public VariableMemoryPoolFactory {
public:
	VariableMemoryPool* create(std::size_t memoryPoolSize, void* memoryPool)
	{
		VariableMemoryPool* p = new TestVariableMemoryPool(memoryPoolSize, memoryPool);
		return p;
	}

	void destroy(VariableMemoryPool* p)
	{
		delete static_cast<TestVariableMemoryPool*>(p);
	}
};

TEST_GROUP(VariableMemoryPoolTest) {
	TestVariableMemoryPoolFactory testFactory;
	VariableMemoryPool* allocator;
	char pool[100];

	void setup()
	{
		OSWrapper::registerVariableMemoryPoolFactory(&testFactory);
	}
	void teardown()
	{
		mock().checkExpectations();
		mock().clear();
	}
};

TEST(VariableMemoryPoolTest, create_destroy)
{
	allocator = VariableMemoryPool::create(sizeof pool, pool);
	CHECK(allocator);
	VariableMemoryPool::destroy(allocator);
}

TEST(VariableMemoryPoolTest, create_no_param_pool)
{
	allocator = VariableMemoryPool::create(sizeof pool);
	CHECK(allocator);
	VariableMemoryPool::destroy(allocator);
}

TEST(VariableMemoryPoolTest, destroy_nullptr)
{
	VariableMemoryPool::destroy(0);
}

TEST(VariableMemoryPoolTest, allocate)
{
	allocator = VariableMemoryPool::create(sizeof pool, pool);
	std::size_t size = 10;
	mock().expectOneCall("allocate").onObject(allocator).withParameter("size", size).andReturnValue(static_cast<void*>(pool));

	void* p = allocator->allocate(size);
	POINTERS_EQUAL(pool, p);

	VariableMemoryPool::destroy(allocator);
}

TEST(VariableMemoryPoolTest, allocate_failed)
{
	allocator = VariableMemoryPool::create(sizeof pool, pool);
	std::size_t size = 10;
	mock().expectOneCall("allocate").onObject(allocator).withParameter("size", size);

	void* p = allocator->allocate(size);
	POINTERS_EQUAL(0, p);

	VariableMemoryPool::destroy(allocator);
}

TEST(VariableMemoryPoolTest, deallocate)
{
	allocator = VariableMemoryPool::create(sizeof pool, pool);
	void* p = pool;
	mock().expectOneCall("deallocate").onObject(allocator).withParameter("p", p);

	allocator->deallocate(p);

	VariableMemoryPool::destroy(allocator);
}

TEST(VariableMemoryPoolTest, deallocate_nullptr)
{
	allocator = VariableMemoryPool::create(sizeof pool, pool);
	void* p = 0;
	mock().expectOneCall("deallocate").onObject(allocator).withParameter("p", p);

	allocator->deallocate(p);

	VariableMemoryPool::destroy(allocator);
}

