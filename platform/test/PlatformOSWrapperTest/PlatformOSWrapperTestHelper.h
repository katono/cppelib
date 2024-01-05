#ifndef PLATFORM_OS_WRAPPER_TEST_HELPER_H_INCLUDED
#define PLATFORM_OS_WRAPPER_TEST_HELPER_H_INCLUDED

#include "OSWrapper/ThreadFactory.h"
#include "OSWrapper/MutexFactory.h"
#include "OSWrapper/EventFlagFactory.h"
#include "OSWrapper/FixedMemoryPoolFactory.h"
#include "OSWrapper/VariableMemoryPoolFactory.h"
#include "OSWrapper/PeriodicTimerFactory.h"
#include "OSWrapper/OneShotTimerFactory.h"

namespace PlatformOSWrapperTestHelper {

void registerOSWrapperFactories();
unsigned long getCurrentTime();
unsigned long getTimeTolerance();
OSWrapper::ThreadFactory* getThreadFactory();

class TestHelper {
public:
	virtual ~TestHelper() {}
	virtual OSWrapper::ThreadFactory* getThreadFactory() = 0;
	virtual OSWrapper::MutexFactory* getMutexFactory() = 0;
	virtual OSWrapper::EventFlagFactory* getEventFlagFactory() = 0;
	virtual OSWrapper::FixedMemoryPoolFactory* getFixedMemoryPoolFactory() = 0;
	virtual OSWrapper::VariableMemoryPoolFactory* getVariableMemoryPoolFactory() = 0;
	virtual OSWrapper::PeriodicTimerFactory* getPeriodicTimerFactory() = 0;
	virtual OSWrapper::OneShotTimerFactory* getOneShotTimerFactory() = 0;
	virtual unsigned long getCurrentTime() = 0;
};

void registerTestHelper(TestHelper* helper);

}

#endif // PLATFORM_OS_WRAPPER_TEST_HELPER_H_INCLUDED
