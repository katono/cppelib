#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "OSWrapper/VariableMemoryPool.h"
#include "ItronOSWrapper/ItronVariableMemoryPoolFactory.h"

using OSWrapper::VariableMemoryPool;
using OSWrapper::VariableMemoryPoolFactory;
using ItronOSWrapper::ItronVariableMemoryPoolFactory;

TEST_GROUP(PlatformVariableMemoryPoolTest) {
	ItronVariableMemoryPoolFactory testFactory;
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

TEST(PlatformVariableMemoryPoolTest, create_destroy)
{
	pool = VariableMemoryPool::create(sizeof poolBuf, poolBuf);
	CHECK(pool);
	VariableMemoryPool::destroy(pool);
}

TEST(PlatformVariableMemoryPoolTest, create_no_param_pool)
{
	pool = VariableMemoryPool::create(sizeof poolBuf);
	CHECK(pool);
	VariableMemoryPool::destroy(pool);
}

TEST(PlatformVariableMemoryPoolTest, destroy_nullptr)
{
	VariableMemoryPool::destroy(0);
}

TEST(PlatformVariableMemoryPoolTest, allocate)
{
	pool = VariableMemoryPool::create(sizeof poolBuf, poolBuf);
	std::size_t size = 10;

	void* p = pool->allocate(size);
	CHECK(p);

	pool->deallocate(p);

	VariableMemoryPool::destroy(pool);
}

TEST(PlatformVariableMemoryPoolTest, allocate_failed)
{
	pool = VariableMemoryPool::create(18);
	std::size_t size = 10;

	void* p = pool->allocate(size);
	CHECK(p);

	void* q = pool->allocate(size);
	CHECK(q == 0);

	pool->deallocate(p);

	q = pool->allocate(size);
	CHECK(q);
	pool->deallocate(q);

	VariableMemoryPool::destroy(pool);
}

TEST(PlatformVariableMemoryPoolTest, deallocate_nullptr)
{
	pool = VariableMemoryPool::create(sizeof poolBuf, poolBuf);
	void* p = 0;

	pool->deallocate(p);

	VariableMemoryPool::destroy(pool);
}

