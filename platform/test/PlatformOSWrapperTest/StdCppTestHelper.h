#ifndef STDCPP_TEST_HELPER_H_INCLUDED
#define STDCPP_TEST_HELPER_H_INCLUDED

#include "PlatformOSWrapperTestHelper.h"
#include "StdCppOSWrapper/StdCppThreadFactory.h"
#include "StdCppOSWrapper/StdCppMutexFactory.h"
#include "StdCppOSWrapper/StdCppEventFlagFactory.h"
#include "StdCppOSWrapper/StdCppFixedMemoryPoolFactory.h"
#include "StdCppOSWrapper/StdCppVariableMemoryPoolFactory.h"
#include "StdCppOSWrapper/StdCppPeriodicTimerFactory.h"
#include "StdCppOSWrapper/StdCppOneShotTimerFactory.h"
#include <chrono>

class StdCppTestHelper : public PlatformOSWrapperTestHelper::TestHelper {
public:
	OSWrapper::ThreadFactory* createThreadFactory()
	{
		return new StdCppOSWrapper::StdCppThreadFactory();
	}

	OSWrapper::MutexFactory* createMutexFactory()
	{
		return new StdCppOSWrapper::StdCppMutexFactory();
	}

	OSWrapper::EventFlagFactory* createEventFlagFactory()
	{
		return new StdCppOSWrapper::StdCppEventFlagFactory();
	}

	OSWrapper::FixedMemoryPoolFactory* createFixedMemoryPoolFactory()
	{
		return new StdCppOSWrapper::StdCppFixedMemoryPoolFactory();
	}

	OSWrapper::VariableMemoryPoolFactory* createVariableMemoryPoolFactory()
	{
		return new StdCppOSWrapper::StdCppVariableMemoryPoolFactory();
	}

	OSWrapper::PeriodicTimerFactory* createPeriodicTimerFactory()
	{
		return new StdCppOSWrapper::StdCppPeriodicTimerFactory();
	}

	OSWrapper::OneShotTimerFactory* createOneShotTimerFactory()
	{
		return new StdCppOSWrapper::StdCppOneShotTimerFactory();
	}

	unsigned long getCurrentTime()
	{
		auto now = std::chrono::system_clock::now();
		auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
		return static_cast<unsigned long>(now_ms.count());
	}
};

#endif // STDCPP_TEST_HELPER_H_INCLUDED
