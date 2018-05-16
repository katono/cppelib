#ifndef OS_WRAPPER_FIXED_MEMORY_POOL_H_INCLUDED
#define OS_WRAPPER_FIXED_MEMORY_POOL_H_INCLUDED

#include <cstddef>

namespace OSWrapper {

class FixedMemoryPoolFactory;

void registerFixedMemoryPoolFactory(FixedMemoryPoolFactory* factory);

class FixedMemoryPool {
protected:
	virtual ~FixedMemoryPool() {}

public:
	static FixedMemoryPool* create(std::size_t blockSize, std::size_t memoryPoolSize, void* memoryPoolAddress = 0);
	static void destroy(FixedMemoryPool* p);

	virtual void* allocate() = 0;
	virtual void deallocate(void* p) = 0;

	virtual std::size_t getBlockSize() const = 0;
};

}

#endif // OS_WRAPPER_FIXED_MEMORY_POOL_H_INCLUDED
