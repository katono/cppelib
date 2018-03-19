#ifndef WINDOWS_OS_WRAPPER_WINDOWS_VARIABLE_ALLOCATOR_H_INCLUDED
#define WINDOWS_OS_WRAPPER_WINDOWS_VARIABLE_ALLOCATOR_H_INCLUDED

#include "OSWrapper/VariableAllocator.h"

namespace WindowsOSWrapper {

class WindowsVariableAllocator : public OSWrapper::VariableAllocator {
public:
	WindowsVariableAllocator() {}
	virtual ~WindowsVariableAllocator() {}

	virtual void* allocate(std::size_t size);
	virtual void deallocate(void* p);
};

}

#endif // WINDOWS_OS_WRAPPER_WINDOWS_VARIABLE_ALLOCATOR_H_INCLUDED
