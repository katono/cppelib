#include "PlatformOSWrapperTestHelper.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/Mutex.h"
#include "OSWrapper/EventFlag.h"
#include "OSWrapper/FixedMemoryPool.h"
#include "OSWrapper/VariableMemoryPool.h"
#include "OSWrapper/PeriodicTimer.h"
#include "OSWrapper/OneShotTimer.h"
#include "Assertion/Assertion.h"

namespace PlatformOSWrapperTestHelper {

static TestHelper* s_helper;
static OSWrapper::ThreadFactory* s_threadFactory;
static OSWrapper::MutexFactory* s_mutexFactory;
static OSWrapper::EventFlagFactory* s_eventFlagFactory;
static OSWrapper::FixedMemoryPoolFactory* s_fixedMemoryPoolFactory;
static OSWrapper::VariableMemoryPoolFactory* s_variableMemoryPoolFactory;
static OSWrapper::PeriodicTimerFactory* s_periodicTimerFactory;
static OSWrapper::OneShotTimerFactory* s_oneShotTimerFactory;

void registerTestHelper(TestHelper* helper)
{
	s_helper = helper;
}

void createAndRegisterOSWrapperFactories()
{
	CHECK_ASSERT(s_helper);
	s_threadFactory = s_helper->createThreadFactory();
	s_mutexFactory = s_helper->createMutexFactory();
	s_eventFlagFactory = s_helper->createEventFlagFactory();
	s_fixedMemoryPoolFactory = s_helper->createFixedMemoryPoolFactory();
	s_variableMemoryPoolFactory = s_helper->createVariableMemoryPoolFactory();
	s_periodicTimerFactory = s_helper->createPeriodicTimerFactory();
	s_oneShotTimerFactory = s_helper->createOneShotTimerFactory();

	OSWrapper::registerThreadFactory(s_threadFactory);
	OSWrapper::registerMutexFactory(s_mutexFactory);
	OSWrapper::registerEventFlagFactory(s_eventFlagFactory);
	OSWrapper::registerFixedMemoryPoolFactory(s_fixedMemoryPoolFactory);
	OSWrapper::registerVariableMemoryPoolFactory(s_variableMemoryPoolFactory);
	OSWrapper::registerPeriodicTimerFactory(s_periodicTimerFactory);
	OSWrapper::registerOneShotTimerFactory(s_oneShotTimerFactory);
}

void destroyOSWrapperFactories()
{
	CHECK_ASSERT(s_helper);
	s_helper->destroyThreadFactory(s_threadFactory);
	s_helper->destroyMutexFactory(s_mutexFactory);
	s_helper->destroyEventFlagFactory(s_eventFlagFactory);
	s_helper->destroyFixedMemoryPoolFactory(s_fixedMemoryPoolFactory);
	s_helper->destroyVariableMemoryPoolFactory(s_variableMemoryPoolFactory);
	s_helper->destroyPeriodicTimerFactory(s_periodicTimerFactory);
	s_helper->destroyOneShotTimerFactory(s_oneShotTimerFactory);

	s_threadFactory = 0;
	s_mutexFactory = 0;
	s_eventFlagFactory = 0;
	s_fixedMemoryPoolFactory = 0;
	s_variableMemoryPoolFactory = 0;
	s_periodicTimerFactory = 0;
	s_oneShotTimerFactory = 0;

	OSWrapper::registerThreadFactory(0);
	OSWrapper::registerMutexFactory(0);
	OSWrapper::registerEventFlagFactory(0);
	OSWrapper::registerFixedMemoryPoolFactory(0);
	OSWrapper::registerVariableMemoryPoolFactory(0);
	OSWrapper::registerPeriodicTimerFactory(0);
	OSWrapper::registerOneShotTimerFactory(0);
}

unsigned long getCurrentTime()
{
	return s_helper->getCurrentTime();
}

unsigned long getTimeTolerance()
{
#if defined(PLATFORM_OS_WINDOWS) || defined(PLATFORM_OS_POSIX) || defined(PLATFORM_OS_STDCPP)
	return 20;
#else
	return 3;
#endif
}

OSWrapper::ThreadFactory* getThreadFactory()
{
	return s_threadFactory;
}

}
