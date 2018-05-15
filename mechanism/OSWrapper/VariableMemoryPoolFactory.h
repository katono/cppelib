#ifndef OS_WRAPPER_VARIABLE_ALLOCATOR_FACTORY_H_INCLUDED
#define OS_WRAPPER_VARIABLE_ALLOCATOR_FACTORY_H_INCLUDED

#include <cstddef>

namespace OSWrapper {

class VariableAllocator;

class VariableAllocatorFactory {
public:
	virtual ~VariableAllocatorFactory() {}
	virtual VariableAllocator* create(std::size_t memoryPoolSize, void* memoryPool) = 0;
	virtual void destroy(VariableAllocator* p) = 0;
};

}

#endif // OS_WRAPPER_VARIABLE_ALLOCATOR_FACTORY_H_INCLUDED
