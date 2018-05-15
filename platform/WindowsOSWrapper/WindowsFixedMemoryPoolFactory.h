#ifndef WINDOWS_OS_WRAPPER_WINDOWS_FIXED_ALLOCATOR_FACTORY_H_INCLUDED
#define WINDOWS_OS_WRAPPER_WINDOWS_FIXED_ALLOCATOR_FACTORY_H_INCLUDED

#include "OSWrapper/FixedAllocatorFactory.h"
#include <mutex>

namespace WindowsOSWrapper {

class WindowsFixedAllocatorFactory : public OSWrapper::FixedAllocatorFactory {
private:
	mutable std::mutex m_mutex;

public:
	WindowsFixedAllocatorFactory();
	virtual ~WindowsFixedAllocatorFactory() {}

private:
	virtual OSWrapper::FixedAllocator* create(std::size_t blockSize, std::size_t memoryPoolSize, void* memoryPool);
	virtual void destroy(OSWrapper::FixedAllocator* p);

	WindowsFixedAllocatorFactory(const WindowsFixedAllocatorFactory&);
	WindowsFixedAllocatorFactory& operator=(const WindowsFixedAllocatorFactory&);
};

}

#endif // WINDOWS_OS_WRAPPER_WINDOWS_FIXED_ALLOCATOR_FACTORY_H_INCLUDED
