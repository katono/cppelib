#ifndef POSIX_TEST_HELPER_H_INCLUDED
#define POSIX_TEST_HELPER_H_INCLUDED

#include "PlatformOSWrapperTestHelper.h"
#include "PosixOSWrapper/PosixThreadFactory.h"
#include "PosixOSWrapper/PosixMutexFactory.h"
#include "PosixOSWrapper/PosixEventFlagFactory.h"
#include "PosixOSWrapper/PosixFixedMemoryPoolFactory.h"
#include "PosixOSWrapper/PosixVariableMemoryPoolFactory.h"
#include "PosixOSWrapper/PosixPeriodicTimerFactory.h"
#include "PosixOSWrapper/PosixOneShotTimerFactory.h"
#include <chrono>

class PosixTestHelper : public PlatformOSWrapperTestHelper::TestHelper {
private:
	PosixOSWrapper::PosixThreadFactory m_threadFactory;
	PosixOSWrapper::PosixMutexFactory m_mutexFactory;
	PosixOSWrapper::PosixEventFlagFactory m_eventFlagFactory;
	PosixOSWrapper::PosixFixedMemoryPoolFactory m_fixedMemoryPoolFactory;
	PosixOSWrapper::PosixVariableMemoryPoolFactory m_variableMemoryPoolFactory;
	PosixOSWrapper::PosixPeriodicTimerFactory m_periodicTimerFactory;
	PosixOSWrapper::PosixOneShotTimerFactory m_oneShotTimerFactory;

public:
	OSWrapper::ThreadFactory* getThreadFactory()
	{
		return &m_threadFactory;
	}

	OSWrapper::MutexFactory* getMutexFactory()
	{
		return &m_mutexFactory;
	}

	OSWrapper::EventFlagFactory* getEventFlagFactory()
	{
		return &m_eventFlagFactory;
	}

	OSWrapper::FixedMemoryPoolFactory* getFixedMemoryPoolFactory()
	{
		return &m_fixedMemoryPoolFactory;
	}

	OSWrapper::VariableMemoryPoolFactory* getVariableMemoryPoolFactory()
	{
		return &m_variableMemoryPoolFactory;
	}

	OSWrapper::PeriodicTimerFactory* getPeriodicTimerFactory()
	{
		return &m_periodicTimerFactory;
	}

	OSWrapper::OneShotTimerFactory* getOneShotTimerFactory()
	{
		return &m_oneShotTimerFactory;
	}

	unsigned long getCurrentTime()
	{
		auto now = std::chrono::system_clock::now();
		auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
		return static_cast<unsigned long>(now_ms.count());
	}
};

#endif // POSIX_TEST_HELPER_H_INCLUDED
