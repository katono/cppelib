#ifndef STDCPP_OS_WRAPPER_STDCPP_FIXED_MEMORY_POOL_FACTORY_H_INCLUDED
#define STDCPP_OS_WRAPPER_STDCPP_FIXED_MEMORY_POOL_FACTORY_H_INCLUDED

#include "OSWrapper/FixedMemoryPoolFactory.h"
#include <mutex>

namespace StdCppOSWrapper {

class StdCppFixedMemoryPoolFactory : public OSWrapper::FixedMemoryPoolFactory {
public:
	StdCppFixedMemoryPoolFactory();
	virtual ~StdCppFixedMemoryPoolFactory() {}

private:
	virtual OSWrapper::FixedMemoryPool* create(std::size_t blockSize, std::size_t memoryPoolSize, void* memoryPoolAddress);
	virtual void destroy(OSWrapper::FixedMemoryPool* p);
	virtual std::size_t getRequiredMemorySize(std::size_t blockSize, std::size_t numBlocks);

	StdCppFixedMemoryPoolFactory(const StdCppFixedMemoryPoolFactory&);
	StdCppFixedMemoryPoolFactory& operator=(const StdCppFixedMemoryPoolFactory&);

	mutable std::mutex m_mutex;
};

}

#endif // STDCPP_OS_WRAPPER_STDCPP_FIXED_MEMORY_POOL_FACTORY_H_INCLUDED
