#include "FixedMemoryPool.h"
#include "FixedMemoryPoolFactory.h"
#include "Assertion/Assertion.h"

namespace OSWrapper {

static FixedMemoryPoolFactory* s_factory = 0;

void registerFixedMemoryPoolFactory(FixedMemoryPoolFactory* factory)
{
	s_factory = factory;
}

FixedMemoryPool* FixedMemoryPool::create(std::size_t blockSize, std::size_t memoryPoolSize, void* memoryPool/*= 0*/)
{
	CHECK_PRECOND(s_factory);
	return s_factory->create(blockSize, memoryPoolSize, memoryPool);
}

void FixedMemoryPool::destroy(FixedMemoryPool* p)
{
	if ((s_factory != 0) && (p != 0)) {
		s_factory->destroy(p);
	}
}

}
