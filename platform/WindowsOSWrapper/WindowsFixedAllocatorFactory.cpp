#include "WindowsFixedAllocatorFactory.h"
#include "OSWrapper/FixedAllocator.h"
#include <cstdlib>

namespace WindowsOSWrapper {

class WindowsFixedAllocator : public OSWrapper::FixedAllocator {
private:
	const std::size_t m_blockSize;

public:
	WindowsFixedAllocator(std::size_t blockSize) : m_blockSize(blockSize) {}
	~WindowsFixedAllocator() {}

	void* allocate()
	{
		return std::malloc(m_blockSize);
	}

	void deallocate(void* p)
	{
		std::free(p);
	}

	std::size_t getBlockSize() const
	{
		return m_blockSize;
	}
};


WindowsFixedAllocatorFactory::WindowsFixedAllocatorFactory()
: m_mutex()
{
}

OSWrapper::FixedAllocator* WindowsFixedAllocatorFactory::create(std::size_t blockSize, std::size_t memoryPoolSize, void* memoryPool)
{
	(void) memoryPoolSize;
	(void) memoryPool;
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
	delete static_cast<WindowsFixedAllocator*>(p);
}

}
