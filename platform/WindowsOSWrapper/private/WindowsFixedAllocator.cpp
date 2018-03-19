#include "WindowsFixedAllocator.h"
#include <cstdlib>

namespace WindowsOSWrapper {

void* WindowsFixedAllocator::allocate()
{
	return std::malloc(m_blockSize);
}

void WindowsFixedAllocator::deallocate(void* p)
{
	std::free(p);
}

}
