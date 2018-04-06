#include "VariableAllocator.h"
#include "VariableAllocatorFactory.h"
#include "Assertion/Assertion.h"

namespace OSWrapper {

static VariableAllocatorFactory* s_factory = 0;

void registerVariableAllocatorFactory(VariableAllocatorFactory* factory)
{
	s_factory = factory;
}

VariableAllocator* VariableAllocator::create(std::size_t memoryPoolSize, void* memoryPool/*= 0*/)
{
	DBC_PRE(s_factory);
	return s_factory->create(memoryPoolSize, memoryPool);
}

void VariableAllocator::destroy(VariableAllocator* p)
{
	if ((s_factory != 0) && (p != 0)) {
		s_factory->destroy(p);
	}
}

}
