#include "StdCppVariableMemoryPoolFactory.h"
#include "OSWrapper/VariableMemoryPool.h"
#include <cstdlib>

namespace StdCppOSWrapper {

class StdCppVariableMemoryPool : public OSWrapper::VariableMemoryPool {
public:
	StdCppVariableMemoryPool() {}
	~StdCppVariableMemoryPool() {}

	void* allocate(std::size_t size)
	{
		return std::malloc(size);
	}

	void deallocate(void* p)
	{
		std::free(p);
	}
};

StdCppVariableMemoryPoolFactory::StdCppVariableMemoryPoolFactory()
: m_mutex()
{
}

OSWrapper::VariableMemoryPool* StdCppVariableMemoryPoolFactory::create(std::size_t memoryPoolSize, void* memoryPoolAddress)
{
	(void) memoryPoolAddress;
	if (memoryPoolSize == 0U) {
		return nullptr;
	}
#ifndef CPPELIB_NO_EXCEPTIONS
	try {
#endif
		std::lock_guard<std::mutex> lock(m_mutex);
		StdCppVariableMemoryPool* p = new StdCppVariableMemoryPool();
		return p;
#ifndef CPPELIB_NO_EXCEPTIONS
	}
	catch (...) {
		return nullptr;
	}
#endif
}

void StdCppVariableMemoryPoolFactory::destroy(OSWrapper::VariableMemoryPool* p)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	delete static_cast<StdCppVariableMemoryPool*>(p);
}

}
