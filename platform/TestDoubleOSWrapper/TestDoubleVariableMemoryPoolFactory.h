#ifndef TEST_DOUBLE_OS_WRAPPER_TEST_DOUBLE_VARIABLE_MEMORY_POOL_FACTORY_H_INCLUDED
#define TEST_DOUBLE_OS_WRAPPER_TEST_DOUBLE_VARIABLE_MEMORY_POOL_FACTORY_H_INCLUDED

#include "OSWrapper/VariableMemoryPool.h"
#include "OSWrapper/VariableMemoryPoolFactory.h"
#include <cstdlib>

namespace TestDoubleOSWrapper {

class TestDoubleVariableMemoryPool : public OSWrapper::VariableMemoryPool {
protected:
	std::size_t m_memoryPoolSize;
	void* m_memoryPoolAddress;

public:
	TestDoubleVariableMemoryPool()
	: m_memoryPoolSize(0U)
	, m_memoryPoolAddress(0)
	{
	}

	void setCreateArgs(std::size_t memoryPoolSize, void* memoryPoolAddress)
	{
		m_memoryPoolSize = memoryPoolSize;
		m_memoryPoolAddress = memoryPoolAddress;
	}

	virtual ~TestDoubleVariableMemoryPool()
	{
	}

	virtual void* allocate(std::size_t size)
	{
		return std::malloc(size);
	}

	virtual void deallocate(void* p)
	{
		std::free(p);
	}
};

template <typename T = TestDoubleVariableMemoryPool>
class TestDoubleVariableMemoryPoolFactory : public OSWrapper::VariableMemoryPoolFactory {
public:
	TestDoubleVariableMemoryPoolFactory() {}
	virtual ~TestDoubleVariableMemoryPoolFactory() {}

	virtual OSWrapper::VariableMemoryPool* create(std::size_t memoryPoolSize, void* memoryPoolAddress)
	{
		T* obj = new T();
		obj->setCreateArgs(memoryPoolSize, memoryPoolAddress);
		return obj;
	}

	virtual void destroy(OSWrapper::VariableMemoryPool* p)
	{
		delete static_cast<T*>(p);
	}

private:
	TestDoubleVariableMemoryPoolFactory(const TestDoubleVariableMemoryPoolFactory&);
	TestDoubleVariableMemoryPoolFactory& operator=(const TestDoubleVariableMemoryPoolFactory&);
};

}

#endif // TEST_DOUBLE_OS_WRAPPER_TEST_DOUBLE_VARIABLE_MEMORY_POOL_FACTORY_H_INCLUDED
