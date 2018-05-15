#include "WindowsVariableAllocatorFactory.h"
#include "OSWrapper/VariableAllocator.h"
#include <cstdlib>

namespace WindowsOSWrapper {

class WindowsVariableAllocator : public OSWrapper::VariableAllocator {
public:
	WindowsVariableAllocator() {}
	~WindowsVariableAllocator() {}

	void* allocate(std::size_t size)
	{
		return std::malloc(size);
	}

	void deallocate(void* p)
	{
		std::free(p);
	}
};

WindowsVariableAllocatorFactory::WindowsVariableAllocatorFactory()
: m_mutex()
{
}

OSWrapper::VariableAllocator* WindowsVariableAllocatorFactory::create(std::size_t memoryPoolSize, void* memoryPool)
{
	(void) memoryPoolSize;
	(void) memoryPool;
	try {
		std::lock_guard<std::mutex> lock(m_mutex);
		WindowsVariableAllocator* p = new WindowsVariableAllocator();
		return p;
	}
	catch (...) {
		return nullptr;
	}
}

void WindowsVariableAllocatorFactory::destroy(OSWrapper::VariableAllocator* p)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	delete static_cast<WindowsVariableAllocator*>(p);
}

}
