#ifndef OS_WRAPPER_FIXED_MEMORY_POOL_FACTORY_H_INCLUDED
#define OS_WRAPPER_FIXED_MEMORY_POOL_FACTORY_H_INCLUDED

#include <cstddef>

namespace OSWrapper {

class FixedMemoryPool;

class FixedMemoryPoolFactory {
public:
	virtual ~FixedMemoryPoolFactory() {}
	virtual FixedMemoryPool* create(std::size_t blockSize, std::size_t memoryPoolSize, void* memoryPoolAddress) = 0;
	virtual void destroy(FixedMemoryPool* p) = 0;
	virtual std::size_t getRequiredMemorySize(std::size_t blockSize, std::size_t numBlocks) = 0;
};

}

#endif // OS_WRAPPER_FIXED_MEMORY_POOL_FACTORY_H_INCLUDED
