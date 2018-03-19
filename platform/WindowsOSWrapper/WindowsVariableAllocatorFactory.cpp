#include "WindowsVariableAllocatorFactory.h"
#include "private/WindowsVariableAllocator.h"

namespace WindowsOSWrapper {

WindowsVariableAllocatorFactory::WindowsVariableAllocatorFactory()
: m_mutex()
{
}

OSWrapper::VariableAllocator* WindowsVariableAllocatorFactory::create(std::size_t memoryPoolSize, void* memoryPool)
{
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
	delete p;
}

}
