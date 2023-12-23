#ifndef TEST_DOUBLE_OS_WRAPPER_TEST_DOUBLE_FIXED_MEMORY_POOL_FACTORY_H_INCLUDED
#define TEST_DOUBLE_OS_WRAPPER_TEST_DOUBLE_FIXED_MEMORY_POOL_FACTORY_H_INCLUDED

#include "OSWrapper/FixedMemoryPool.h"
#include "OSWrapper/FixedMemoryPoolFactory.h"
#include <cstdlib>

namespace TestDoubleOSWrapper {

class TestDoubleFixedMemoryPool : public OSWrapper::FixedMemoryPool {
protected:
	std::size_t m_blockSize;
	std::size_t m_memoryPoolSize;
	void* m_memoryPoolAddress;

public:
	TestDoubleFixedMemoryPool()
	: m_blockSize(0U)
	, m_memoryPoolSize(0U)
	, m_memoryPoolAddress(0)
	{
	}

	void setCreateArgs(std::size_t blockSize, std::size_t memoryPoolSize, void* memoryPoolAddress)
	{
		m_blockSize = blockSize;
		m_memoryPoolSize = memoryPoolSize;
		m_memoryPoolAddress = memoryPoolAddress;
	}

	virtual ~TestDoubleFixedMemoryPool()
	{
	}

	virtual void* allocate()
	{
		return std::malloc(m_blockSize);
	}

	virtual void deallocate(void* p)
	{
		std::free(p);
	}

	virtual std::size_t getBlockSize() const
	{
		return m_blockSize;
	}
};

template <typename T = TestDoubleFixedMemoryPool>
class TestDoubleFixedMemoryPoolFactory : public OSWrapper::FixedMemoryPoolFactory {
public:
	TestDoubleFixedMemoryPoolFactory() {}
	virtual ~TestDoubleFixedMemoryPoolFactory() {}

	virtual OSWrapper::FixedMemoryPool* create(std::size_t blockSize, std::size_t memoryPoolSize, void* memoryPoolAddress)
	{
		T* obj = new T();
		obj->setCreateArgs(blockSize, memoryPoolSize, memoryPoolAddress);
		return obj;
	}

	virtual void destroy(OSWrapper::FixedMemoryPool* p)
	{
		delete static_cast<T*>(p);
	}

	virtual std::size_t getRequiredMemorySize(std::size_t blockSize, std::size_t numBlocks)
	{
		return blockSize * numBlocks;
	}

private:
	TestDoubleFixedMemoryPoolFactory(const TestDoubleFixedMemoryPoolFactory&);
	TestDoubleFixedMemoryPoolFactory& operator=(const TestDoubleFixedMemoryPoolFactory&);
};

}

#endif // TEST_DOUBLE_OS_WRAPPER_TEST_DOUBLE_FIXED_MEMORY_POOL_FACTORY_H_INCLUDED
