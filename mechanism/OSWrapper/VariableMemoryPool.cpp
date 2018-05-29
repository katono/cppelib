#include "VariableMemoryPool.h"
#include "VariableMemoryPoolFactory.h"
#include "Assertion/Assertion.h"

namespace OSWrapper {

static VariableMemoryPoolFactory* s_factory = 0;

void registerVariableMemoryPoolFactory(VariableMemoryPoolFactory* factory)
{
	s_factory = factory;
}

VariableMemoryPool* VariableMemoryPool::create(std::size_t memoryPoolSize, void* memoryPoolAddress/*= 0*/)
{
	CHECK_ASSERT(s_factory);
	return s_factory->create(memoryPoolSize, memoryPoolAddress);
}

void VariableMemoryPool::destroy(VariableMemoryPool* p)
{
	if ((s_factory != 0) && (p != 0)) {
		s_factory->destroy(p);
	}
}

}
