#ifndef ITRON_OS_WRAPPER_ITRON_VARIABLE_MEMORY_POOL_FACTORY_H_INCLUDED
#define ITRON_OS_WRAPPER_ITRON_VARIABLE_MEMORY_POOL_FACTORY_H_INCLUDED

#include "OSWrapper/VariableMemoryPoolFactory.h"
#include "OSWrapper/VariableMemoryPool.h"
#include "Container/Array.h"
#include "kernel.h"

#ifndef MAX_VARIABLE_MEMORY_POOLS
#define MAX_VARIABLE_MEMORY_POOLS (256U)
#endif

namespace ItronOSWrapper {

class ItronVariableMemoryPoolFactory : public OSWrapper::VariableMemoryPoolFactory {
private:
	class ItronVariableMemoryPool : public OSWrapper::VariableMemoryPool {
	private:
		ID m_mplId;

	public:
		ItronVariableMemoryPool() : m_mplId(0) {}
		~ItronVariableMemoryPool();

		bool init(std::size_t memoryPoolSize, void* memoryPoolAddress);
		void deinit();
		bool isCreated() { return m_mplId > 0; }

		void* allocate(std::size_t size);
		void deallocate(void* p);
	};
	typedef Container::Array<ItronVariableMemoryPool, MAX_VARIABLE_MEMORY_POOLS> VariableMemoryPoolArray;
	VariableMemoryPoolArray m_pools;
	ID m_mutexId;

	virtual OSWrapper::VariableMemoryPool* create(std::size_t memoryPoolSize, void* memoryPoolAddress);
	virtual void destroy(OSWrapper::VariableMemoryPool* p);

	ItronVariableMemoryPoolFactory(const ItronVariableMemoryPoolFactory&);
	ItronVariableMemoryPoolFactory& operator=(const ItronVariableMemoryPoolFactory&);

public:
	ItronVariableMemoryPoolFactory();
	virtual ~ItronVariableMemoryPoolFactory();
};

}

#endif // ITRON_OS_WRAPPER_ITRON_VARIABLE_MEMORY_POOL_FACTORY_H_INCLUDED
