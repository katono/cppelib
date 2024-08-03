#include "OSWrapper/VariableMemoryPool.h"

#include "PlatformOSWrapperTestHelper.h"

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

namespace PlatformVariableMemoryPoolTest {

using OSWrapper::VariableMemoryPool;
using OSWrapper::VariableMemoryPoolFactory;

TEST_GROUP(PlatformVariableMemoryPoolTest) {
	void setup()
	{
		PlatformOSWrapperTestHelper::createAndRegisterOSWrapperFactories();
	}
	void teardown()
	{
		PlatformOSWrapperTestHelper::destroyOSWrapperFactories();

		mock().checkExpectations();
		mock().clear();
	}
};

TEST(PlatformVariableMemoryPoolTest, create_destroy)
{
	double poolBuf[100];
	VariableMemoryPool* pool = VariableMemoryPool::create(sizeof poolBuf, poolBuf);
	CHECK(pool);
	VariableMemoryPool::destroy(pool);
}

TEST(PlatformVariableMemoryPoolTest, create_no_param_pool)
{
	VariableMemoryPool* pool = VariableMemoryPool::create(100);
	CHECK(pool);
	VariableMemoryPool::destroy(pool);
}

TEST(PlatformVariableMemoryPoolTest, create_failed_parameter)
{
	VariableMemoryPool* pool = VariableMemoryPool::create(0);
	CHECK(!pool);
}

TEST(PlatformVariableMemoryPoolTest, destroy_nullptr)
{
	VariableMemoryPool::destroy(0);
}

TEST(PlatformVariableMemoryPoolTest, allocate)
{
	double poolBuf[100];
	VariableMemoryPool* pool = VariableMemoryPool::create(sizeof poolBuf, poolBuf);
	CHECK(pool);
	std::size_t size = 10;

	void* p = pool->allocate(size);
	CHECK(p);

	pool->deallocate(p);

	VariableMemoryPool::destroy(pool);
}

#if defined(PLATFORM_OS_ITRON)
TEST(PlatformVariableMemoryPoolTest, allocate_failed)
{
	VariableMemoryPool* pool = VariableMemoryPool::create(18);
	CHECK(pool);
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
#endif

TEST(PlatformVariableMemoryPoolTest, deallocate_nullptr)
{
	double poolBuf[100];
	VariableMemoryPool* pool = VariableMemoryPool::create(sizeof poolBuf, poolBuf);
	CHECK(pool);
	void* p = 0;

	pool->deallocate(p);

	VariableMemoryPool::destroy(pool);
}

} // namespace PlatformVariableMemoryPoolTest
