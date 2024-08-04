#include "StdCppFixedMemoryPoolFactory.h"
#include "OSWrapper/FixedMemoryPool.h"
#include "Container/IntrusiveList.h"
#include <condition_variable>
#include <chrono>
#include <vector>
#include <cstdlib>

namespace StdCppOSWrapper {

using Container::IntrusiveListNode;
using Container::IntrusiveList;
using OSWrapper::Timeout;

namespace {

inline std::size_t getAlignedSize(std::size_t size)
{
	return ((size + (sizeof(double) - 1U)) & (~(sizeof(double) - 1U)));
}

};

class StdCppFixedMemoryPool : public OSWrapper::FixedMemoryPool {
private:
	const std::size_t m_blockSize;
	const std::size_t m_maxBlocks;

	struct MemoryNode : public IntrusiveListNode {
		void* data;
		MemoryNode(): data() {}
		~MemoryNode() {}
	};
	IntrusiveList<MemoryNode> m_freeList;
	IntrusiveList<MemoryNode> m_usedList;
	std::vector<MemoryNode> m_realNodeArray;
	std::vector<char> m_memoryPoolBuffer;

	mutable std::mutex m_mutex;
	std::condition_variable m_cond;

public:
	explicit StdCppFixedMemoryPool(std::size_t blockSize, std::size_t maxBlocks, std::size_t memoryPoolSize, void* memoryPoolAddress)
	: m_blockSize(blockSize), m_maxBlocks(maxBlocks)
	, m_freeList(), m_usedList(), m_realNodeArray(maxBlocks), m_memoryPoolBuffer()
	, m_mutex(), m_cond()
	{
		char* p = static_cast<char*>(memoryPoolAddress);
		if (p == nullptr) {
			m_memoryPoolBuffer.resize(memoryPoolSize);
			p = &m_memoryPoolBuffer[0];
		}
		const std::size_t alignedBlockSize = getAlignedSize(blockSize);
		for (std::size_t i = 0U; i < maxBlocks; i++) {
			m_realNodeArray[i].data = p;
			p += alignedBlockSize;
			m_freeList.push_back(m_realNodeArray[i]);
		}
	}

	~StdCppFixedMemoryPool()
	{
	}

	void* allocate()
	{
		return std::malloc(m_blockSize);
	}

	void deallocate(void* p)
	{
		if (p == nullptr) {
			return;
		}
		std::lock_guard<std::mutex> lock(m_mutex);
		for (auto it = m_usedList.begin(); it != m_usedList.end(); ++it) {
			if (it->data == p) {
				// release memory block to free list
				m_freeList.splice(m_freeList.end(), m_usedList, it);
				m_cond.notify_all();
				return;
			}
		}
		// If p is not included in used list, p is a pointer that std::malloc() returned
		std::free(p);
	}

	std::size_t getBlockSize() const
	{
		return m_blockSize;
	}

	OSWrapper::Error allocateMemory(void** memory)
	{
		return timedAllocateMemory(memory, Timeout::FOREVER);
	}

	OSWrapper::Error tryAllocateMemory(void** memory)
	{
		return timedAllocateMemory(memory, Timeout::POLLING);
	}

	OSWrapper::Error timedAllocateMemory(void** memory, Timeout tmout)
	{
		if (memory == nullptr) {
			return OSWrapper::InvalidParameter;
		}

		std::unique_lock<std::mutex> lock(m_mutex);
		if (tmout == Timeout::FOREVER) {
			m_cond.wait(lock, [&] { return !m_freeList.empty(); });
		} else {
			if (!m_cond.wait_for(lock, std::chrono::milliseconds(tmout),
						[&] { return !m_freeList.empty(); })) {
				return OSWrapper::TimedOut;
			}
		}
		// get memory block from free list
		auto it = m_freeList.begin();
		void* p = it->data;
		m_usedList.splice(m_usedList.end(), m_freeList, it);
		*memory = p;
		return OSWrapper::OK;
	}

	std::size_t getNumberOfAvailableBlocks() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_freeList.size();
	}

	std::size_t getMaxNumberOfBlocks() const
	{
		return m_maxBlocks;
	}

};


StdCppFixedMemoryPoolFactory::StdCppFixedMemoryPoolFactory()
: m_mutex()
{
}

OSWrapper::FixedMemoryPool* StdCppFixedMemoryPoolFactory::create(std::size_t blockSize, std::size_t memoryPoolSize, void* memoryPoolAddress)
{
	if ((blockSize == 0U) || (memoryPoolSize == 0U)) {
		return nullptr;
	}
	const std::size_t maxBlocks = memoryPoolSize / getAlignedSize(blockSize);
	if (maxBlocks == 0U) {
		return nullptr;
	}
#ifndef CPPELIB_NO_EXCEPTIONS
	try {
#endif
		std::lock_guard<std::mutex> lock(m_mutex);
		StdCppFixedMemoryPool* p = new StdCppFixedMemoryPool(blockSize, maxBlocks, memoryPoolSize, memoryPoolAddress);
		return p;
#ifndef CPPELIB_NO_EXCEPTIONS
	}
	catch (...) {
		return nullptr;
	}
#endif
}

void StdCppFixedMemoryPoolFactory::destroy(OSWrapper::FixedMemoryPool* p)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	delete static_cast<StdCppFixedMemoryPool*>(p);
}

std::size_t StdCppFixedMemoryPoolFactory::getRequiredMemorySize(std::size_t blockSize, std::size_t numBlocks)
{
	return getAlignedSize(blockSize) * numBlocks;
}

}
