#ifndef WINDOWS_OS_WRAPPER_WINDOWS_FIXED_MEMORY_POOL_FACTORY_H_INCLUDED
#define WINDOWS_OS_WRAPPER_WINDOWS_FIXED_MEMORY_POOL_FACTORY_H_INCLUDED

#include "OSWrapper/FixedMemoryPoolFactory.h"
#include <mutex>

namespace WindowsOSWrapper {

class WindowsFixedMemoryPoolFactory : public OSWrapper::FixedMemoryPoolFactory {
public:
	WindowsFixedMemoryPoolFactory();
	virtual ~WindowsFixedMemoryPoolFactory() {}

private:
	virtual OSWrapper::FixedMemoryPool* create(std::size_t blockSize, std::size_t memoryPoolSize, void* memoryPoolAddress);
	virtual void destroy(OSWrapper::FixedMemoryPool* p);
	virtual std::size_t getRequiredMemorySize(std::size_t blockSize, std::size_t numBlocks);

	WindowsFixedMemoryPoolFactory(const WindowsFixedMemoryPoolFactory&);
	WindowsFixedMemoryPoolFactory& operator=(const WindowsFixedMemoryPoolFactory&);

	mutable std::mutex m_mutex;
};

}

#endif // WINDOWS_OS_WRAPPER_WINDOWS_FIXED_MEMORY_POOL_FACTORY_H_INCLUDED
