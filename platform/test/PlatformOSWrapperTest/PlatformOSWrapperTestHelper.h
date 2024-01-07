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

void createAndRegisterOSWrapperFactories();
void destroyOSWrapperFactories();
unsigned long getCurrentTime();
unsigned long getTimeTolerance();
OSWrapper::ThreadFactory* getThreadFactory();

class TestHelper {
public:
	virtual ~TestHelper() {}

	virtual OSWrapper::ThreadFactory* createThreadFactory() = 0;
	virtual OSWrapper::MutexFactory* createMutexFactory() = 0;
	virtual OSWrapper::EventFlagFactory* createEventFlagFactory() = 0;
	virtual OSWrapper::FixedMemoryPoolFactory* createFixedMemoryPoolFactory() = 0;
	virtual OSWrapper::VariableMemoryPoolFactory* createVariableMemoryPoolFactory() = 0;
	virtual OSWrapper::PeriodicTimerFactory* createPeriodicTimerFactory() = 0;
	virtual OSWrapper::OneShotTimerFactory* createOneShotTimerFactory() = 0;

	virtual void destroyThreadFactory(OSWrapper::ThreadFactory* factory) = 0;
	virtual void destroyMutexFactory(OSWrapper::MutexFactory* factory) = 0;
	virtual void destroyEventFlagFactory(OSWrapper::EventFlagFactory* factory) = 0;
	virtual void destroyFixedMemoryPoolFactory(OSWrapper::FixedMemoryPoolFactory* factory) = 0;
	virtual void destroyVariableMemoryPoolFactory(OSWrapper::VariableMemoryPoolFactory* factory) = 0;
	virtual void destroyPeriodicTimerFactory(OSWrapper::PeriodicTimerFactory* factory) = 0;
	virtual void destroyOneShotTimerFactory(OSWrapper::OneShotTimerFactory* factory) = 0;

	virtual unsigned long getCurrentTime() = 0;
};

void registerTestHelper(TestHelper* helper);

}

#endif // PLATFORM_OS_WRAPPER_TEST_HELPER_H_INCLUDED
