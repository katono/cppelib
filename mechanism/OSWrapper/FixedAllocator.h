#ifndef OS_WRAPPER_FIXED_ALLOCATOR_H_INCLUDED
#define OS_WRAPPER_FIXED_ALLOCATOR_H_INCLUDED

#include <cstddef>

namespace OSWrapper {

class FixedAllocatorFactory;

void registerFixedAllocatorFactory(FixedAllocatorFactory* factory);

class FixedAllocator {
protected:
	virtual ~FixedAllocator() {}

public:
	static FixedAllocator* create(std::size_t blockSize, std::size_t memoryPoolSize, void* memoryPool = 0);
	static void destroy(FixedAllocator* p);

	virtual void* allocate() = 0;
	virtual void deallocate(void* p) = 0;

	virtual std::size_t getBlockSize() const = 0;
};

}

#endif // OS_WRAPPER_FIXED_ALLOCATOR_H_INCLUDED
