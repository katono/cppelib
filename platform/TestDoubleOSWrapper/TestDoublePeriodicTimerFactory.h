#ifndef TEST_DOUBLE_OS_WRAPPER_TEST_DOUBLE_PERIODIC_TIMER_FACTORY_H_INCLUDED
#define TEST_DOUBLE_OS_WRAPPER_TEST_DOUBLE_PERIODIC_TIMER_FACTORY_H_INCLUDED

#include "OSWrapper/Runnable.h"
#include "OSWrapper/PeriodicTimer.h"
#include "OSWrapper/PeriodicTimerFactory.h"

namespace TestDoubleOSWrapper {

class TestDoublePeriodicTimer : public OSWrapper::PeriodicTimer {
protected:
	OSWrapper::Runnable* m_runnable;
	unsigned long m_periodInMillis;
	const char* m_name;

public:
	TestDoublePeriodicTimer()
	: PeriodicTimer(0)
	, m_runnable(0)
	, m_periodInMillis(0U)
	, m_name(0)
	{
	}

	void setCreateArgs(OSWrapper::Runnable* r, unsigned long periodInMillis, const char* name)
	{
		m_runnable = r;
		m_periodInMillis = periodInMillis;
		m_name = name;
	}

	virtual ~TestDoublePeriodicTimer()
	{
	}

	virtual void start()
	{
	}

	virtual void stop()
	{
	}

	virtual bool isStarted() const
	{
		return false;
	}

	virtual unsigned long getPeriodInMillis() const
	{
		return m_periodInMillis;
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

template <typename T = TestDoublePeriodicTimer>
class TestDoublePeriodicTimerFactory : public OSWrapper::PeriodicTimerFactory {
public:
	TestDoublePeriodicTimerFactory() {}
	virtual ~TestDoublePeriodicTimerFactory() {}

	virtual OSWrapper::PeriodicTimer* create(OSWrapper::Runnable* r, unsigned long periodInMillis, const char* name)
	{
		T* obj = new T();
		obj->setCreateArgs(r, periodInMillis, name);
		return obj;
	}

	virtual void destroy(OSWrapper::PeriodicTimer* t)
	{
		delete static_cast<T*>(t);
	}

private:
	TestDoublePeriodicTimerFactory(const TestDoublePeriodicTimerFactory&);
	TestDoublePeriodicTimerFactory& operator=(const TestDoublePeriodicTimerFactory&);
};

}

#endif // TEST_DOUBLE_OS_WRAPPER_TEST_DOUBLE_PERIODIC_TIMER_FACTORY_H_INCLUDED
