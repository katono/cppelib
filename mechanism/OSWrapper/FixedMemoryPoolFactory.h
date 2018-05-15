#ifndef OS_WRAPPER_FIXED_ALLOCATOR_FACTORY_H_INCLUDED
#define OS_WRAPPER_FIXED_ALLOCATOR_FACTORY_H_INCLUDED

#include <cstddef>

namespace OSWrapper {

class FixedAllocator;

class FixedAllocatorFactory {
public:
	virtual ~FixedAllocatorFactory() {}
	virtual FixedAllocator* create(std::size_t blockSize, std::size_t memoryPoolSize, void* memoryPool) = 0;
	virtual void destroy(FixedAllocator* p) = 0;
};

}

#endif // OS_WRAPPER_FIXED_ALLOCATOR_FACTORY_H_INCLUDED
