#ifndef WINDOWS_OS_WRAPPER_WINDOWS_VARIABLE_ALLOCATOR_FACTORY_H_INCLUDED
#define WINDOWS_OS_WRAPPER_WINDOWS_VARIABLE_ALLOCATOR_FACTORY_H_INCLUDED

#include "OSWrapper/VariableAllocatorFactory.h"
#include <mutex>

namespace WindowsOSWrapper {

class WindowsVariableAllocatorFactory : public OSWrapper::VariableAllocatorFactory {
private:
	mutable std::mutex m_mutex;

public:
	WindowsVariableAllocatorFactory();
	virtual ~WindowsVariableAllocatorFactory() {}

private:
	virtual OSWrapper::VariableAllocator* create(std::size_t memoryPoolSize, void* memoryPool);
	virtual void destroy(OSWrapper::VariableAllocator* p);
};

}

#endif // WINDOWS_OS_WRAPPER_WINDOWS_VARIABLE_ALLOCATOR_FACTORY_H_INCLUDED
