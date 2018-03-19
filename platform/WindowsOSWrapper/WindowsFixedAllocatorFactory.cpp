#include "WindowsFixedAllocatorFactory.h"
#include "private/WindowsFixedAllocator.h"

namespace WindowsOSWrapper {

WindowsFixedAllocatorFactory::WindowsFixedAllocatorFactory()
: m_mutex()
{
}

OSWrapper::FixedAllocator* WindowsFixedAllocatorFactory::create(std::size_t blockSize, std::size_t memoryPoolSize, void* memoryPool)
{
	try {
		std::lock_guard<std::mutex> lock(m_mutex);
		WindowsFixedAllocator* p = new WindowsFixedAllocator(blockSize);
		return p;
	}
	catch (...) {
		return nullptr;
	}
}

void WindowsFixedAllocatorFactory::destroy(OSWrapper::FixedAllocator* p)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	delete p;
}

}
