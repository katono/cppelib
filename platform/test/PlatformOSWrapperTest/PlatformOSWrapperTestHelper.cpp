#include "PlatformOSWrapperTestHelper.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/Mutex.h"
#include "OSWrapper/EventFlag.h"
#include "OSWrapper/FixedMemoryPool.h"
#include "OSWrapper/VariableMemoryPool.h"
#include "OSWrapper/PeriodicTimer.h"
#include "OSWrapper/OneShotTimer.h"

namespace PlatformOSWrapperTestHelper {

static TestHelper* s_helper;

void registerTestHelper(TestHelper* helper)
{
	s_helper = helper;
}

void registerOSWrapperFactories()
{
	OSWrapper::registerThreadFactory(s_helper->getThreadFactory());
	OSWrapper::registerMutexFactory(s_helper->getMutexFactory());
	OSWrapper::registerEventFlagFactory(s_helper->getEventFlagFactory());
	OSWrapper::registerFixedMemoryPoolFactory(s_helper->getFixedMemoryPoolFactory());
	OSWrapper::registerVariableMemoryPoolFactory(s_helper->getVariableMemoryPoolFactory());
	OSWrapper::registerPeriodicTimerFactory(s_helper->getPeriodicTimerFactory());
	OSWrapper::registerOneShotTimerFactory(s_helper->getOneShotTimerFactory());
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
	return s_helper->getThreadFactory();
}

}
