#ifndef WINDOWS_OS_WRAPPER_WINDOWS_VARIABLE_MEMORY_POOL_FACTORY_H_INCLUDED
#define WINDOWS_OS_WRAPPER_WINDOWS_VARIABLE_MEMORY_POOL_FACTORY_H_INCLUDED

#include "OSWrapper/VariableMemoryPoolFactory.h"
#include <mutex>

namespace WindowsOSWrapper {

class WindowsVariableMemoryPoolFactory : public OSWrapper::VariableMemoryPoolFactory {
private:
	mutable std::mutex m_mutex;

public:
	WindowsVariableMemoryPoolFactory();
	virtual ~WindowsVariableMemoryPoolFactory() {}

private:
	virtual OSWrapper::VariableMemoryPool* create(std::size_t memoryPoolSize, void* memoryPool);
	virtual void destroy(OSWrapper::VariableMemoryPool* p);

	WindowsVariableMemoryPoolFactory(const WindowsVariableMemoryPoolFactory&);
	WindowsVariableMemoryPoolFactory& operator=(const WindowsVariableMemoryPoolFactory&);
};

}

#endif // WINDOWS_OS_WRAPPER_WINDOWS_VARIABLE_MEMORY_POOL_FACTORY_H_INCLUDED
