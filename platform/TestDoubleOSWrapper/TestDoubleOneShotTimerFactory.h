#ifndef TEST_DOUBLE_OS_WRAPPER_TEST_DOUBLE_ONESHOT_TIMER_FACTORY_H_INCLUDED
#define TEST_DOUBLE_OS_WRAPPER_TEST_DOUBLE_ONESHOT_TIMER_FACTORY_H_INCLUDED

#include "OSWrapper/Runnable.h"
#include "OSWrapper/OneShotTimer.h"
#include "OSWrapper/OneShotTimerFactory.h"

namespace TestDoubleOSWrapper {

class TestDoubleOneShotTimer : public OSWrapper::OneShotTimer {
protected:
	OSWrapper::Runnable* m_runnable;
	const char* m_name;

public:
	TestDoubleOneShotTimer()
	: OneShotTimer(0)
	, m_runnable(0)
	, m_name(0)
	{
	}

	void setCreateArgs(OSWrapper::Runnable* r, const char* name)
	{
		m_runnable = r;
		m_name = name;
	}

	virtual ~TestDoubleOneShotTimer()
	{
	}

	virtual void start(unsigned long timeInMillis)
	{
		(void) timeInMillis;
	}

	virtual void stop()
	{
	}

	virtual bool isStarted() const
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
};

template <typename T = TestDoubleOneShotTimer>
class TestDoubleOneShotTimerFactory : public OSWrapper::OneShotTimerFactory {
public:
	TestDoubleOneShotTimerFactory() {}
	virtual ~TestDoubleOneShotTimerFactory() {}

	virtual OSWrapper::OneShotTimer* create(OSWrapper::Runnable* r, const char* name)
	{
		T* obj = new T();
		obj->setCreateArgs(r, name);
		return obj;
	}

	virtual void destroy(OSWrapper::OneShotTimer* t)
	{
		delete static_cast<T*>(t);
	}

private:
	TestDoubleOneShotTimerFactory(const TestDoubleOneShotTimerFactory&);
	TestDoubleOneShotTimerFactory& operator=(const TestDoubleOneShotTimerFactory&);
};

}

#endif // TEST_DOUBLE_OS_WRAPPER_TEST_DOUBLE_ONESHOT_TIMER_FACTORY_H_INCLUDED
