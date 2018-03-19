#include "WindowsVariableAllocator.h"
#include <cstdlib>

namespace WindowsOSWrapper {

void* WindowsVariableAllocator::allocate(std::size_t size)
{
	return std::malloc(size);
}

void WindowsVariableAllocator::deallocate(void* p)
{
	std::free(p);
}

}
