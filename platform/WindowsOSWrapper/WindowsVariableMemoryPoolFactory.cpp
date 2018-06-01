#include "WindowsVariableMemoryPoolFactory.h"
#include "OSWrapper/VariableMemoryPool.h"
#include <cstdlib>

namespace WindowsOSWrapper {

class WindowsVariableMemoryPool : public OSWrapper::VariableMemoryPool {
public:
	WindowsVariableMemoryPool() {}
	~WindowsVariableMemoryPool() {}

	void* allocate(std::size_t size)
	{
		return std::malloc(size);
	}

	void deallocate(void* p)
	{
		std::free(p);
	}
};

WindowsVariableMemoryPoolFactory::WindowsVariableMemoryPoolFactory()
: m_mutex()
{
}

OSWrapper::VariableMemoryPool* WindowsVariableMemoryPoolFactory::create(std::size_t memoryPoolSize, void* memoryPoolAddress)
{
	(void) memoryPoolAddress;
	if (memoryPoolSize == 0U) {
		return nullptr;
	}
	try {
		std::lock_guard<std::mutex> lock(m_mutex);
		WindowsVariableMemoryPool* p = new WindowsVariableMemoryPool();
		return p;
	}
	catch (...) {
		return nullptr;
	}
}

void WindowsVariableMemoryPoolFactory::destroy(OSWrapper::VariableMemoryPool* p)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	delete static_cast<WindowsVariableMemoryPool*>(p);
}

}
