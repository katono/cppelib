#include "WindowsFixedMemoryPoolFactory.h"
#include "OSWrapper/FixedMemoryPool.h"
#include <cstdlib>

namespace WindowsOSWrapper {

class WindowsFixedMemoryPool : public OSWrapper::FixedMemoryPool {
private:
	const std::size_t m_blockSize;

public:
	explicit WindowsFixedMemoryPool(std::size_t blockSize) : m_blockSize(blockSize) {}
	~WindowsFixedMemoryPool() {}

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


WindowsFixedMemoryPoolFactory::WindowsFixedMemoryPoolFactory()
: m_mutex()
{
}

OSWrapper::FixedMemoryPool* WindowsFixedMemoryPoolFactory::create(std::size_t blockSize, std::size_t memoryPoolSize, void* memoryPoolAddress)
{
	(void) memoryPoolAddress;
	if ((blockSize == 0U) || (memoryPoolSize == 0U)) {
		return nullptr;
	}
	try {
		std::lock_guard<std::mutex> lock(m_mutex);
		WindowsFixedMemoryPool* p = new WindowsFixedMemoryPool(blockSize);
		return p;
	}
	catch (...) {
		return nullptr;
	}
}

void WindowsFixedMemoryPoolFactory::destroy(OSWrapper::FixedMemoryPool* p)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	delete static_cast<WindowsFixedMemoryPool*>(p);
}

std::size_t WindowsFixedMemoryPoolFactory::getRequiredMemorySize(std::size_t blockSize, std::size_t numBlocks)
{
	return blockSize * numBlocks;
}

}
