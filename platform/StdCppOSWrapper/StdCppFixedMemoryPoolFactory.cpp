#include "StdCppFixedMemoryPoolFactory.h"
#include "OSWrapper/FixedMemoryPool.h"
#include <cstdlib>

namespace StdCppOSWrapper {

class StdCppFixedMemoryPool : public OSWrapper::FixedMemoryPool {
private:
	const std::size_t m_blockSize;

public:
	explicit StdCppFixedMemoryPool(std::size_t blockSize) : m_blockSize(blockSize) {}
	~StdCppFixedMemoryPool() {}

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


StdCppFixedMemoryPoolFactory::StdCppFixedMemoryPoolFactory()
: m_mutex()
{
}

OSWrapper::FixedMemoryPool* StdCppFixedMemoryPoolFactory::create(std::size_t blockSize, std::size_t memoryPoolSize, void* memoryPoolAddress)
{
	(void) memoryPoolAddress;
	if ((blockSize == 0U) || (memoryPoolSize == 0U)) {
		return nullptr;
	}
	try {
		std::lock_guard<std::mutex> lock(m_mutex);
		StdCppFixedMemoryPool* p = new StdCppFixedMemoryPool(blockSize);
		return p;
	}
	catch (...) {
		return nullptr;
	}
}

void StdCppFixedMemoryPoolFactory::destroy(OSWrapper::FixedMemoryPool* p)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	delete static_cast<StdCppFixedMemoryPool*>(p);
}

std::size_t StdCppFixedMemoryPoolFactory::getRequiredMemorySize(std::size_t blockSize, std::size_t numBlocks)
{
	return blockSize * numBlocks;
}

}
