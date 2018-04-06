#include "FixedAllocator.h"
#include "FixedAllocatorFactory.h"
#include "Assertion/Assertion.h"

namespace OSWrapper {

static FixedAllocatorFactory* s_factory = 0;

void registerFixedAllocatorFactory(FixedAllocatorFactory* factory)
{
	s_factory = factory;
}

FixedAllocator* FixedAllocator::create(std::size_t blockSize, std::size_t memoryPoolSize, void* memoryPool/*= 0*/)
{
	DBC_PRE(s_factory);
	return s_factory->create(blockSize, memoryPoolSize, memoryPool);
}

void FixedAllocator::destroy(FixedAllocator* p)
{
	if ((s_factory != 0) && (p != 0)) {
		s_factory->destroy(p);
	}
}

}
