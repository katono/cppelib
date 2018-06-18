#include "OSWrapper/FixedMemoryPool.h"

#ifdef PLATFORM_OS_WINDOWS
#include "WindowsOSWrapper/WindowsFixedMemoryPoolFactory.h"
typedef WindowsOSWrapper::WindowsFixedMemoryPoolFactory PlatformFixedMemoryPoolFactory;
#elif PLATFORM_OS_ITRON
#include "ItronOSWrapper/ItronFixedMemoryPoolFactory.h"
typedef ItronOSWrapper::ItronFixedMemoryPoolFactory PlatformFixedMemoryPoolFactory;
#endif

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

using OSWrapper::FixedMemoryPool;
using OSWrapper::FixedMemoryPoolFactory;

TEST_GROUP(PlatformFixedMemoryPoolTest) {
	PlatformFixedMemoryPoolFactory testFixedMemoryPoolFactory;
	FixedMemoryPool* pool;
	double poolBuf[100];

	void setup()
	{
		OSWrapper::registerFixedMemoryPoolFactory(&testFixedMemoryPoolFactory);
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

#ifndef PLATFORM_OS_WINDOWS
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
#endif

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

