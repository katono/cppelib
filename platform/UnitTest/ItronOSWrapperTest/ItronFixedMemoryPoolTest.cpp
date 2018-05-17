#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "OSWrapper/FixedMemoryPool.h"
#include "ItronOSWrapper/ItronFixedMemoryPoolFactory.h"

using OSWrapper::FixedMemoryPool;
using OSWrapper::FixedMemoryPoolFactory;
using ItronOSWrapper::ItronFixedMemoryPoolFactory;

TEST_GROUP(ItronFixedMemoryPoolTest) {
	ItronFixedMemoryPoolFactory testFactory;
	FixedMemoryPool* pool;
	double poolBuf[100];

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

TEST(ItronFixedMemoryPoolTest, create_destroy)
{
	pool = FixedMemoryPool::create(16, sizeof poolBuf, poolBuf);
	CHECK(pool);
	FixedMemoryPool::destroy(pool);
}

TEST(ItronFixedMemoryPoolTest, create_no_param_pool)
{
	pool = FixedMemoryPool::create(16, sizeof poolBuf);
	CHECK(pool);
	FixedMemoryPool::destroy(pool);
}

TEST(ItronFixedMemoryPoolTest, create_failed_parameter)
{
	pool = FixedMemoryPool::create(0, 100);
	CHECK(!pool);

	pool = FixedMemoryPool::create(16, 0);
	CHECK(!pool);
}

TEST(ItronFixedMemoryPoolTest, destroy_nullptr)
{
	FixedMemoryPool::destroy(0);
}

TEST(ItronFixedMemoryPoolTest, allocate_deallocate)
{
	pool = FixedMemoryPool::create(16, sizeof poolBuf, poolBuf);
	CHECK(pool);

	void* p = pool->allocate();
	CHECK(p);

	pool->deallocate(p);

	FixedMemoryPool::destroy(pool);
}

TEST(ItronFixedMemoryPoolTest, allocate_failed)
{
	pool = FixedMemoryPool::create(16, 16);
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

TEST(ItronFixedMemoryPoolTest, deallocate_nullptr)
{
	pool = FixedMemoryPool::create(16, sizeof poolBuf, poolBuf);
	void* p = 0;

	pool->deallocate(p);

	FixedMemoryPool::destroy(pool);
}

TEST(ItronFixedMemoryPoolTest, getBlockSize)
{
	pool = FixedMemoryPool::create(16, sizeof poolBuf, poolBuf);

	LONGS_EQUAL(16, pool->getBlockSize());

	FixedMemoryPool::destroy(pool);
}

