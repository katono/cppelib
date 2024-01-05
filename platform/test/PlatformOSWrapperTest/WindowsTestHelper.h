#ifndef WINDOWS_TEST_HELPER_H_INCLUDED
#define WINDOWS_TEST_HELPER_H_INCLUDED

#include "PlatformOSWrapperTestHelper.h"
#include "WindowsOSWrapper/WindowsThreadFactory.h"
#include "WindowsOSWrapper/WindowsMutexFactory.h"
#include "WindowsOSWrapper/WindowsEventFlagFactory.h"
#include "WindowsOSWrapper/WindowsFixedMemoryPoolFactory.h"
#include "WindowsOSWrapper/WindowsVariableMemoryPoolFactory.h"
#include "WindowsOSWrapper/WindowsPeriodicTimerFactory.h"
#include "WindowsOSWrapper/WindowsOneShotTimerFactory.h"
#include <chrono>

class WindowsTestHelper : public PlatformOSWrapperTestHelper::TestHelper {
private:
	WindowsOSWrapper::WindowsThreadFactory m_threadFactory;
	WindowsOSWrapper::WindowsMutexFactory m_mutexFactory;
	WindowsOSWrapper::WindowsEventFlagFactory m_eventFlagFactory;
	WindowsOSWrapper::WindowsFixedMemoryPoolFactory m_fixedMemoryPoolFactory;
	WindowsOSWrapper::WindowsVariableMemoryPoolFactory m_variableMemoryPoolFactory;
	WindowsOSWrapper::WindowsPeriodicTimerFactory m_periodicTimerFactory;
	WindowsOSWrapper::WindowsOneShotTimerFactory m_oneShotTimerFactory;

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

#endif // WINDOWS_TEST_HELPER_H_INCLUDED
