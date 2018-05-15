#ifndef OS_WRAPPER_VARIABLE_ALLOCATOR_H_INCLUDED
#define OS_WRAPPER_VARIABLE_ALLOCATOR_H_INCLUDED

#include <cstddef>

namespace OSWrapper {

class VariableAllocatorFactory;

void registerVariableAllocatorFactory(VariableAllocatorFactory* factory);

class VariableAllocator {
protected:
	virtual ~VariableAllocator() {}

public:
	static VariableAllocator* create(std::size_t memoryPoolSize, void* memoryPool = 0);
	static void destroy(VariableAllocator* p);

	virtual void* allocate(std::size_t size) = 0;
	virtual void deallocate(void* p) = 0;

};

}

#endif // OS_WRAPPER_VARIABLE_ALLOCATOR_H_INCLUDED
