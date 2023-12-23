#ifndef TEST_DOUBLE_OS_WRAPPER_TEST_DOUBLE_THREAD_FACTORY_H_INCLUDED
#define TEST_DOUBLE_OS_WRAPPER_TEST_DOUBLE_THREAD_FACTORY_H_INCLUDED

#include "OSWrapper/Runnable.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/ThreadFactory.h"

namespace TestDoubleOSWrapper {

class TestDoubleThread : public OSWrapper::Thread {
protected:
	OSWrapper::Runnable* m_runnable;
	int m_priority;
	int m_initialPriority;
	std::size_t m_stackSize;
	void* m_stackAddress;
	const char* m_name;

public:
	TestDoubleThread()
	: Thread(0)
	, m_runnable(0)
	, m_priority(0)
	, m_initialPriority(0)
	, m_stackSize(0U)
	, m_stackAddress(0)
	, m_name(0)
	{
	}

	void setCreateArgs(OSWrapper::Runnable* r, int priority, std::size_t stackSize, void* stackAddress, const char* name)
	{
		m_runnable = r;
		m_priority = priority;
		m_initialPriority = priority;
		m_stackSize = stackSize;
		m_stackAddress = stackAddress;
		m_name = name;
	}

	virtual ~TestDoubleThread() {}

	virtual void start()
	{
	}

	virtual OSWrapper::Error wait()
	{
		return OSWrapper::OK;
	}

	virtual OSWrapper::Error tryWait()
	{
		return OSWrapper::OK;
	}

	virtual OSWrapper::Error timedWait(OSWrapper::Timeout tmout)
	{
		(void) tmout;
		return OSWrapper::OK;
	}

	virtual bool isFinished() const
	{
		return false;
	}

	virtual void setName(const char* name)
	{
		m_name = name;
	}

	virtual const char* getName() const
	{
		return m_name;
	}

	virtual void setPriority(int priority)
	{
		m_priority = priority;
	}

	virtual int getPriority() const
	{
		return m_priority;
	}

	virtual int getInitialPriority() const
	{
		return m_initialPriority;
	}

	virtual std::size_t getStackSize() const
	{
		return m_stackSize;
	}

	virtual void* getNativeHandle()
	{
		return 0;
	}
};

template <typename T = TestDoubleThread>
class TestDoubleThreadFactory : public OSWrapper::ThreadFactory {
public:
	TestDoubleThreadFactory() {}
	virtual ~TestDoubleThreadFactory() {}

	virtual OSWrapper::Thread* create(OSWrapper::Runnable* r, int priority, std::size_t stackSize, void* stackAddress, const char* name)
	{
		T* obj = new T();
		obj->setCreateArgs(r, priority, stackSize, stackAddress, name);
		return obj;
	}

	virtual void destroy(OSWrapper::Thread* t)
	{
		delete static_cast<T*>(t);
	}

	virtual void sleep(unsigned long millis)
	{
		(void) millis;
	}

	virtual void yield()
	{
	}

	virtual OSWrapper::Thread* getCurrentThread()
	{
		return 0;
	}

	virtual int getMaxPriority() const
	{
		return 0;
	}

	virtual int getMinPriority() const
	{
		return 0;
	}

	virtual int getHighestPriority() const
	{
		return 0;
	}

	virtual int getLowestPriority() const
	{
		return 0;
	}

private:
	TestDoubleThreadFactory(const TestDoubleThreadFactory&);
	TestDoubleThreadFactory& operator=(const TestDoubleThreadFactory&);
};

}

#endif // TEST_DOUBLE_OS_WRAPPER_TEST_DOUBLE_THREAD_FACTORY_H_INCLUDED
