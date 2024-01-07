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
public:
	OSWrapper::ThreadFactory* createThreadFactory()
	{
		return new PosixOSWrapper::PosixThreadFactory();
	}

	OSWrapper::MutexFactory* createMutexFactory()
	{
		return new PosixOSWrapper::PosixMutexFactory();
	}

	OSWrapper::EventFlagFactory* createEventFlagFactory()
	{
		return new PosixOSWrapper::PosixEventFlagFactory();
	}

	OSWrapper::FixedMemoryPoolFactory* createFixedMemoryPoolFactory()
	{
		return new PosixOSWrapper::PosixFixedMemoryPoolFactory();
	}

	OSWrapper::VariableMemoryPoolFactory* createVariableMemoryPoolFactory()
	{
		return new PosixOSWrapper::PosixVariableMemoryPoolFactory();
	}

	OSWrapper::PeriodicTimerFactory* createPeriodicTimerFactory()
	{
		return new PosixOSWrapper::PosixPeriodicTimerFactory();
	}

	OSWrapper::OneShotTimerFactory* createOneShotTimerFactory()
	{
		return new PosixOSWrapper::PosixOneShotTimerFactory();
	}

	void destroyThreadFactory(OSWrapper::ThreadFactory* factory)
	{
		delete factory;
	}

	void destroyMutexFactory(OSWrapper::MutexFactory* factory)
	{
		delete factory;
	}

	void destroyEventFlagFactory(OSWrapper::EventFlagFactory* factory)
	{
		delete factory;
	}

	void destroyFixedMemoryPoolFactory(OSWrapper::FixedMemoryPoolFactory* factory)
	{
		delete factory;
	}

	void destroyVariableMemoryPoolFactory(OSWrapper::VariableMemoryPoolFactory* factory)
	{
		delete factory;
	}

	void destroyPeriodicTimerFactory(OSWrapper::PeriodicTimerFactory* factory)
	{
		delete factory;
	}

	void destroyOneShotTimerFactory(OSWrapper::OneShotTimerFactory* factory)
	{
		delete factory;
	}

	unsigned long getCurrentTime()
	{
		auto now = std::chrono::system_clock::now();
		auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
		return static_cast<unsigned long>(now_ms.count());
	}

	unsigned long getTimeTolerance()
	{
		return 20;
	}
};

#endif // POSIX_TEST_HELPER_H_INCLUDED
