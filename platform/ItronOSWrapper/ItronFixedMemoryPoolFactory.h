#ifndef ITRON_OS_WRAPPER_ITRON_FIXED_MEMORY_POOL_FACTORY_H_INCLUDED
#define ITRON_OS_WRAPPER_ITRON_FIXED_MEMORY_POOL_FACTORY_H_INCLUDED

#include "OSWrapper/FixedMemoryPoolFactory.h"
#include "OSWrapper/FixedMemoryPool.h"
#include "Container/Array.h"
#include "kernel.h"

#ifndef MAX_FIXED_MEMORY_POOLS
#define MAX_FIXED_MEMORY_POOLS (256U)
#endif

namespace ItronOSWrapper {

class ItronFixedMemoryPoolFactory : public OSWrapper::FixedMemoryPoolFactory {
private:
	class ItronFixedMemoryPool : public OSWrapper::FixedMemoryPool {
	private:
		std::size_t m_blockSize;
		ID m_mpfId;

	public:
		ItronFixedMemoryPool() : m_blockSize(0U), m_mpfId(0) {}
		~ItronFixedMemoryPool();

		bool init(std::size_t blockSize, std::size_t memoryPoolSize, void* memoryPoolAddress);
		void deinit();
		bool isCreated() { return m_mpfId > 0; }

		void* allocate();
		void deallocate(void* p);
		std::size_t getBlockSize() const { return m_blockSize; }
	};
	typedef Container::Array<ItronFixedMemoryPool, MAX_FIXED_MEMORY_POOLS> FixedMemoryPoolArray;
	FixedMemoryPoolArray m_pools;
	ID m_mutexId;

	virtual OSWrapper::FixedMemoryPool* create(std::size_t blockSize, std::size_t memoryPoolSize, void* memoryPoolAddress);
	virtual void destroy(OSWrapper::FixedMemoryPool* p);
	virtual std::size_t getRequiredMemorySize(std::size_t blockSize, std::size_t numBlocks);

	ItronFixedMemoryPoolFactory(const ItronFixedMemoryPoolFactory&);
	ItronFixedMemoryPoolFactory& operator=(const ItronFixedMemoryPoolFactory&);

public:
	ItronFixedMemoryPoolFactory();
	virtual ~ItronFixedMemoryPoolFactory();
};

}

#endif // ITRON_OS_WRAPPER_ITRON_FIXED_MEMORY_POOL_FACTORY_H_INCLUDED
