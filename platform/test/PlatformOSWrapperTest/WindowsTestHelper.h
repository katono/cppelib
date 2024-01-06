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
public:
	OSWrapper::ThreadFactory* createThreadFactory()
	{
		return new WindowsOSWrapper::WindowsThreadFactory();
	}

	OSWrapper::MutexFactory* createMutexFactory()
	{
		return new WindowsOSWrapper::WindowsMutexFactory();
	}

	OSWrapper::EventFlagFactory* createEventFlagFactory()
	{
		return new WindowsOSWrapper::WindowsEventFlagFactory();
	}

	OSWrapper::FixedMemoryPoolFactory* createFixedMemoryPoolFactory()
	{
		return new WindowsOSWrapper::WindowsFixedMemoryPoolFactory();
	}

	OSWrapper::VariableMemoryPoolFactory* createVariableMemoryPoolFactory()
	{
		return new WindowsOSWrapper::WindowsVariableMemoryPoolFactory();
	}

	OSWrapper::PeriodicTimerFactory* createPeriodicTimerFactory()
	{
		return new WindowsOSWrapper::WindowsPeriodicTimerFactory();
	}

	OSWrapper::OneShotTimerFactory* createOneShotTimerFactory()
	{
		return new WindowsOSWrapper::WindowsOneShotTimerFactory();
	}

	unsigned long getCurrentTime()
	{
		auto now = std::chrono::system_clock::now();
		auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
		return static_cast<unsigned long>(now_ms.count());
	}
};

#endif // WINDOWS_TEST_HELPER_H_INCLUDED
