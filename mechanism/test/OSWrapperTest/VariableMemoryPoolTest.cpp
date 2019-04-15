#include "OSWrapper/VariableMemoryPool.h"
#include "OSWrapper/VariableMemoryPoolFactory.h"
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

namespace VariableMemoryPoolTest {

using OSWrapper::VariableMemoryPool;
using OSWrapper::VariableMemoryPoolFactory;

class TestVariableMemoryPool : public VariableMemoryPool {
private:
	std::size_t m_memoryPoolSize;
	void* m_memoryPoolAddress;
public:
	TestVariableMemoryPool(std::size_t memoryPoolSize, void* memoryPoolAddress)
	: m_memoryPoolSize(memoryPoolSize), m_memoryPoolAddress(memoryPoolAddress) {}
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
	VariableMemoryPool* create(std::size_t memoryPoolSize, void* memoryPoolAddress)
	{
		VariableMemoryPool* p = new TestVariableMemoryPool(memoryPoolSize, memoryPoolAddress);
		return p;
	}

	void destroy(VariableMemoryPool* p)
	{
		delete static_cast<TestVariableMemoryPool*>(p);
	}
};

TEST_GROUP(VariableMemoryPoolTest) {
	TestVariableMemoryPoolFactory testFactory;
	VariableMemoryPool* pool;
	double poolBuf[100];

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
	pool = VariableMemoryPool::create(sizeof poolBuf, poolBuf);
	CHECK(pool);
	VariableMemoryPool::destroy(pool);
}

TEST(VariableMemoryPoolTest, create_no_param_pool)
{
	pool = VariableMemoryPool::create(sizeof poolBuf);
	CHECK(pool);
	VariableMemoryPool::destroy(pool);
}

TEST(VariableMemoryPoolTest, destroy_nullptr)
{
	VariableMemoryPool::destroy(0);
}

TEST(VariableMemoryPoolTest, allocate)
{
	pool = VariableMemoryPool::create(sizeof poolBuf, poolBuf);
	std::size_t size = 10;
	mock().expectOneCall("allocate").onObject(pool).withParameter("size", size).andReturnValue(static_cast<void*>(poolBuf));

	void* p = pool->allocate(size);
	POINTERS_EQUAL(poolBuf, p);

	VariableMemoryPool::destroy(pool);
}

TEST(VariableMemoryPoolTest, allocate_failed)
{
	pool = VariableMemoryPool::create(sizeof poolBuf, poolBuf);
	std::size_t size = 10;
	mock().expectOneCall("allocate").onObject(pool).withParameter("size", size);

	void* p = pool->allocate(size);
	POINTERS_EQUAL(0, p);

	VariableMemoryPool::destroy(pool);
}

TEST(VariableMemoryPoolTest, deallocate)
{
	pool = VariableMemoryPool::create(sizeof poolBuf, poolBuf);
	void* p = poolBuf;
	mock().expectOneCall("deallocate").onObject(pool).withParameter("p", p);

	pool->deallocate(p);

	VariableMemoryPool::destroy(pool);
}

TEST(VariableMemoryPoolTest, deallocate_nullptr)
{
	pool = VariableMemoryPool::create(sizeof poolBuf, poolBuf);
	void* p = 0;
	mock().expectOneCall("deallocate").onObject(pool).withParameter("p", p);

	pool->deallocate(p);

	VariableMemoryPool::destroy(pool);
}

} // namespace VariableMemoryPoolTest
