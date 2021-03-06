#include "OSWrapper/VariableMemoryPool.h"

#if defined(PLATFORM_OS_WINDOWS)
#include "WindowsOSWrapper/WindowsVariableMemoryPoolFactory.h"
typedef WindowsOSWrapper::WindowsVariableMemoryPoolFactory PlatformVariableMemoryPoolFactory;
#elif defined(PLATFORM_OS_POSIX)
#include "PosixOSWrapper/PosixVariableMemoryPoolFactory.h"
typedef PosixOSWrapper::PosixVariableMemoryPoolFactory PlatformVariableMemoryPoolFactory;
#elif defined(PLATFORM_OS_STDCPP)
#include "StdCppOSWrapper/StdCppVariableMemoryPoolFactory.h"
typedef StdCppOSWrapper::StdCppVariableMemoryPoolFactory PlatformVariableMemoryPoolFactory;
#elif defined(PLATFORM_OS_ITRON)
#include "ItronOSWrapper/ItronVariableMemoryPoolFactory.h"
typedef ItronOSWrapper::ItronVariableMemoryPoolFactory PlatformVariableMemoryPoolFactory;
#endif

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

namespace PlatformVariableMemoryPoolTest {

using OSWrapper::VariableMemoryPool;
using OSWrapper::VariableMemoryPoolFactory;

TEST_GROUP(PlatformVariableMemoryPoolTest) {
	PlatformVariableMemoryPoolFactory testVariableMemoryPoolFactory;
	VariableMemoryPool* pool;
	double poolBuf[100];

	void setup()
	{
		OSWrapper::registerVariableMemoryPoolFactory(&testVariableMemoryPoolFactory);
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

TEST(PlatformVariableMemoryPoolTest, create_failed_parameter)
{
	pool = VariableMemoryPool::create(0);
	CHECK(!pool);
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

#if defined(PLATFORM_OS_ITRON)
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
#endif

TEST(PlatformVariableMemoryPoolTest, deallocate_nullptr)
{
	pool = VariableMemoryPool::create(sizeof poolBuf, poolBuf);
	void* p = 0;

	pool->deallocate(p);

	VariableMemoryPool::destroy(pool);
}

} // namespace PlatformVariableMemoryPoolTest
