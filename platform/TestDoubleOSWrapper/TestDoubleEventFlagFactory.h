#ifndef TEST_DOUBLE_OS_WRAPPER_TEST_DOUBLE_EVENT_FLAG_FACTORY_H_INCLUDED
#define TEST_DOUBLE_OS_WRAPPER_TEST_DOUBLE_EVENT_FLAG_FACTORY_H_INCLUDED

#include "OSWrapper/EventFlag.h"
#include "OSWrapper/EventFlagFactory.h"

namespace TestDoubleOSWrapper {

class TestDoubleEventFlag : public OSWrapper::EventFlag {
protected:
	bool m_autoReset;

public:
	TestDoubleEventFlag()
	: m_autoReset()
	{
	}

	void setCreateArgs(bool autoReset)
	{
		m_autoReset = autoReset;
	}

	virtual ~TestDoubleEventFlag()
	{
	}

	virtual OSWrapper::Error waitAny()
	{
		return OSWrapper::OK;
	}

	virtual OSWrapper::Error waitOne(std::size_t pos)
	{
		(void) pos;
		return OSWrapper::OK;
	}

	virtual OSWrapper::Error wait(OSWrapper::EventFlag::Pattern bitPattern, OSWrapper::EventFlag::Mode waitMode, OSWrapper::EventFlag::Pattern* releasedPattern)
	{
		(void) bitPattern;
		(void) waitMode;
		(void) releasedPattern;
		return OSWrapper::OK;
	}

	virtual OSWrapper::Error tryWaitAny()
	{
		return OSWrapper::OK;
	}

	virtual OSWrapper::Error tryWaitOne(std::size_t pos)
	{
		(void) pos;
		return OSWrapper::OK;
	}

	virtual OSWrapper::Error tryWait(OSWrapper::EventFlag::Pattern bitPattern, OSWrapper::EventFlag::Mode waitMode, OSWrapper::EventFlag::Pattern* releasedPattern)
	{
		(void) bitPattern;
		(void) waitMode;
		(void) releasedPattern;
		return OSWrapper::OK;
	}

	virtual OSWrapper::Error timedWaitAny(OSWrapper::Timeout tmout)
	{
		(void) tmout;
		return OSWrapper::OK;
	}

	virtual OSWrapper::Error timedWaitOne(std::size_t pos, OSWrapper::Timeout tmout)
	{
		(void) pos;
		(void) tmout;
		return OSWrapper::OK;
	}

	virtual OSWrapper::Error timedWait(OSWrapper::EventFlag::Pattern bitPattern, OSWrapper::EventFlag::Mode waitMode, OSWrapper::EventFlag::Pattern* releasedPattern, OSWrapper::Timeout tmout)
	{
		(void) bitPattern;
		(void) waitMode;
		(void) releasedPattern;
		(void) tmout;
		return OSWrapper::OK;
	}

	virtual OSWrapper::Error setAll()
	{
		return OSWrapper::OK;
	}

	virtual OSWrapper::Error setOne(std::size_t pos)
	{
		(void) pos;
		return OSWrapper::OK;
	}

	virtual OSWrapper::Error set(OSWrapper::EventFlag::Pattern bitPattern)
	{
		(void) bitPattern;
		return OSWrapper::OK;
	}

	virtual OSWrapper::Error resetAll()
	{
		return OSWrapper::OK;
	}

	virtual OSWrapper::Error resetOne(std::size_t pos)
	{
		(void) pos;
		return OSWrapper::OK;
	}

	virtual OSWrapper::Error reset(OSWrapper::EventFlag::Pattern bitPattern)
	{
		(void) bitPattern;
		return OSWrapper::OK;
	}

	virtual OSWrapper::EventFlag::Pattern getCurrentPattern() const
	{
		return OSWrapper::EventFlag::Pattern();
	}
};

template <typename T = TestDoubleEventFlag>
class TestDoubleEventFlagFactory : public OSWrapper::EventFlagFactory {
public:
	TestDoubleEventFlagFactory() {}
	virtual ~TestDoubleEventFlagFactory() {}

	virtual OSWrapper::EventFlag* create(bool autoReset)
	{
		T* obj = new T();
		obj->setCreateArgs(autoReset);
		return obj;
	}

	virtual void destroy(OSWrapper::EventFlag* e)
	{
		delete static_cast<T*>(e);
	}

private:
	TestDoubleEventFlagFactory(const TestDoubleEventFlagFactory&);
	TestDoubleEventFlagFactory& operator=(const TestDoubleEventFlagFactory&);
};

}

#endif // TEST_DOUBLE_OS_WRAPPER_TEST_DOUBLE_EVENT_FLAG_FACTORY_H_INCLUDED
