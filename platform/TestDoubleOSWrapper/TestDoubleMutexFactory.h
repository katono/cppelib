#ifndef TEST_DOUBLE_OS_WRAPPER_TEST_DOUBLE_MUTEX_FACTORY_H_INCLUDED
#define TEST_DOUBLE_OS_WRAPPER_TEST_DOUBLE_MUTEX_FACTORY_H_INCLUDED

#include "OSWrapper/Mutex.h"
#include "OSWrapper/MutexFactory.h"

namespace TestDoubleOSWrapper {

class TestDoubleMutex : public OSWrapper::Mutex {
protected:
	int m_priorityCeiling;

public:
	TestDoubleMutex()
	: m_priorityCeiling(0)
	{
	}

	void setCreateArgs(int priorityCeiling)
	{
		m_priorityCeiling = priorityCeiling;
	}

	virtual ~TestDoubleMutex()
	{
	}

	virtual OSWrapper::Error lock()
	{
		return OSWrapper::OK;
	}

	virtual OSWrapper::Error tryLock()
	{
		return OSWrapper::OK;
	}

	virtual OSWrapper::Error timedLock(OSWrapper::Timeout tmout)
	{
		(void) tmout;
		return OSWrapper::OK;
	}

	virtual OSWrapper::Error unlock()
	{
		return OSWrapper::OK;
	}
};

template <typename T = TestDoubleMutex>
class TestDoubleMutexFactory : public OSWrapper::MutexFactory {
public:
	TestDoubleMutexFactory() {}
	virtual ~TestDoubleMutexFactory() {}

	virtual OSWrapper::Mutex* create()
	{
		return create(0);
	}

	virtual OSWrapper::Mutex* create(int priorityCeiling)
	{
		T* obj = new T();
		obj->setCreateArgs(priorityCeiling);
		return obj;
	}

	virtual void destroy(OSWrapper::Mutex* m)
	{
		delete static_cast<T*>(m);
	}

private:
	TestDoubleMutexFactory(const TestDoubleMutexFactory&);
	TestDoubleMutexFactory& operator=(const TestDoubleMutexFactory&);
};

}

#endif // TEST_DOUBLE_OS_WRAPPER_TEST_DOUBLE_MUTEX_FACTORY_H_INCLUDED
