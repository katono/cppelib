#ifndef OS_WRAPPER_VARIABLE_MEMORY_POOL_H_INCLUDED
#define OS_WRAPPER_VARIABLE_MEMORY_POOL_H_INCLUDED

#include <cstddef>

namespace OSWrapper {

class VariableMemoryPoolFactory;

void registerVariableMemoryPoolFactory(VariableMemoryPoolFactory* factory);

class VariableMemoryPool {
protected:
	virtual ~VariableMemoryPool() {}

public:
	static VariableMemoryPool* create(std::size_t memoryPoolSize, void* memoryPoolAddress = 0);
	static void destroy(VariableMemoryPool* p);

	virtual void* allocate(std::size_t size) = 0;
	virtual void deallocate(void* p) = 0;

};

}

#endif // OS_WRAPPER_VARIABLE_MEMORY_POOL_H_INCLUDED
