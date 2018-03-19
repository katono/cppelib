#ifndef WINDOWS_OS_WRAPPER_WINDOWS_FIXED_ALLOCATOR_H_INCLUDED
#define WINDOWS_OS_WRAPPER_WINDOWS_FIXED_ALLOCATOR_H_INCLUDED

#include "OSWrapper/FixedAllocator.h"

namespace WindowsOSWrapper {

class WindowsFixedAllocator : public OSWrapper::FixedAllocator {
private:
	const std::size_t m_blockSize;

public:
	WindowsFixedAllocator(std::size_t blockSize) : m_blockSize(blockSize) {}
	virtual ~WindowsFixedAllocator() {}

	virtual void* allocate();
	virtual void deallocate(void* p);
};

}

#endif // WINDOWS_OS_WRAPPER_WINDOWS_FIXED_ALLOCATOR_H_INCLUDED
