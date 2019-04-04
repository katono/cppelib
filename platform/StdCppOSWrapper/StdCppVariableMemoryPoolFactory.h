#ifndef STDCPP_OS_WRAPPER_STDCPP_VARIABLE_MEMORY_POOL_FACTORY_H_INCLUDED
#define STDCPP_OS_WRAPPER_STDCPP_VARIABLE_MEMORY_POOL_FACTORY_H_INCLUDED

#include "OSWrapper/VariableMemoryPoolFactory.h"
#include <mutex>

namespace StdCppOSWrapper {

class StdCppVariableMemoryPoolFactory : public OSWrapper::VariableMemoryPoolFactory {
public:
	StdCppVariableMemoryPoolFactory();
	virtual ~StdCppVariableMemoryPoolFactory() {}

private:
	virtual OSWrapper::VariableMemoryPool* create(std::size_t memoryPoolSize, void* memoryPoolAddress);
	virtual void destroy(OSWrapper::VariableMemoryPool* p);

	StdCppVariableMemoryPoolFactory(const StdCppVariableMemoryPoolFactory&);
	StdCppVariableMemoryPoolFactory& operator=(const StdCppVariableMemoryPoolFactory&);

	mutable std::mutex m_mutex;
};

}

#endif // STDCPP_OS_WRAPPER_STDCPP_VARIABLE_MEMORY_POOL_FACTORY_H_INCLUDED
