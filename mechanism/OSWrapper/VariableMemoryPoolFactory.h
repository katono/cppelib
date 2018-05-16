#ifndef OS_WRAPPER_VARIABLE_MEMORY_POOL_FACTORY_H_INCLUDED
#define OS_WRAPPER_VARIABLE_MEMORY_POOL_FACTORY_H_INCLUDED

#include <cstddef>

namespace OSWrapper {

class VariableMemoryPool;

class VariableMemoryPoolFactory {
public:
	virtual ~VariableMemoryPoolFactory() {}
	virtual VariableMemoryPool* create(std::size_t memoryPoolSize, void* memoryPoolAddress) = 0;
	virtual void destroy(VariableMemoryPool* p) = 0;
};

}

#endif // OS_WRAPPER_VARIABLE_MEMORY_POOL_FACTORY_H_INCLUDED
