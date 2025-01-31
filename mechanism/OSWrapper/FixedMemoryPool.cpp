#include "FixedMemoryPool.h"
#include "FixedMemoryPoolFactory.h"
#include "Assertion/Assertion.h"

namespace OSWrapper {

static FixedMemoryPoolFactory* s_factory = 0;

void registerFixedMemoryPoolFactory(FixedMemoryPoolFactory* factory)
{
	s_factory = factory;
}

FixedMemoryPool* FixedMemoryPool::create(std::size_t blockSize, std::size_t memoryPoolSize, void* memoryPoolAddress/*= 0*/)
{
	CHECK_ASSERT(s_factory);
	return s_factory->create(blockSize, memoryPoolSize, memoryPoolAddress);
}

void FixedMemoryPool::destroy(FixedMemoryPool* p)
{
	if ((s_factory != 0) && (p != 0)) {
		s_factory->destroy(p);
	}
}

std::size_t FixedMemoryPool::getRequiredMemorySize(std::size_t blockSize, std::size_t numBlocks)
{
	CHECK_ASSERT(s_factory);
	return s_factory->getRequiredMemorySize(blockSize, numBlocks);
}

Error FixedMemoryPool::allocateMemory(void** memory)
{
	(void) memory;
	return OtherError;
}

Error FixedMemoryPool::tryAllocateMemory(void** memory)
{
	(void) memory;
	return OtherError;
}

Error FixedMemoryPool::timedAllocateMemory(void** memory, Timeout tmout)
{
	(void) memory;
	(void) tmout;
	return OtherError;
}

std::size_t FixedMemoryPool::getNumberOfAvailableBlocks() const
{
	return 0U;
}

std::size_t FixedMemoryPool::getMaxNumberOfBlocks() const
{
	return 0U;
}

}
