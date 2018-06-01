#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "OSWrapper/FixedMemoryPool.h"
#include "ItronOSWrapper/ItronFixedMemoryPoolFactory.h"

using OSWrapper::FixedMemoryPool;
using OSWrapper::FixedMemoryPoolFactory;
using ItronOSWrapper::ItronFixedMemoryPoolFactory;

TEST_GROUP(PlatformFixedMemoryPoolTest) {
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

TEST(PlatformFixedMemoryPoolTest, create_destroy)
{
	pool = FixedMemoryPool::create(16, sizeof poolBuf, poolBuf);
	CHECK(pool);
	FixedMemoryPool::destroy(pool);
}

TEST(PlatformFixedMemoryPoolTest, create_no_param_pool)
{
	pool = FixedMemoryPool::create(16, sizeof poolBuf);
	CHECK(pool);
	FixedMemoryPool::destroy(pool);
}

TEST(PlatformFixedMemoryPoolTest, create_failed_parameter)
{
	pool = FixedMemoryPool::create(0, 100);
	CHECK(!pool);

	pool = FixedMemoryPool::create(16, 0);
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
	pool = FixedMemoryPool::create(16, sizeof poolBuf, poolBuf);
	CHECK(pool);

	void* p = pool->allocate();
	CHECK(p);

	pool->deallocate(p);

	FixedMemoryPool::destroy(pool);
}

TEST(PlatformFixedMemoryPoolTest, allocate_failed)
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

TEST(PlatformFixedMemoryPoolTest, deallocate_nullptr)
{
	pool = FixedMemoryPool::create(16, sizeof poolBuf, poolBuf);
	void* p = 0;

	pool->deallocate(p);

	FixedMemoryPool::destroy(pool);
}

TEST(PlatformFixedMemoryPoolTest, getBlockSize)
{
	pool = FixedMemoryPool::create(16, sizeof poolBuf, poolBuf);

	LONGS_EQUAL(16, pool->getBlockSize());

	FixedMemoryPool::destroy(pool);
}

