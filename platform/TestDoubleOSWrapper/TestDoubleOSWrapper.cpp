#include "TestDoubleOSWrapper.h"
#include "TestDoubleThreadFactory.h"
#include "TestDoubleMutexFactory.h"
#include "TestDoubleEventFlagFactory.h"
#include "TestDoubleFixedMemoryPoolFactory.h"
#include "TestDoubleVariableMemoryPoolFactory.h"
#include "TestDoublePeriodicTimerFactory.h"
#include "TestDoubleOneShotTimerFactory.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/Mutex.h"
#include "OSWrapper/EventFlag.h"
#include "OSWrapper/FixedMemoryPool.h"
#include "OSWrapper/VariableMemoryPool.h"
#include "OSWrapper/PeriodicTimer.h"
#include "OSWrapper/OneShotTimer.h"

/*!
 * @brief Implementation of OSWrapper for Test Double
 *
 * TestDoubleOSWrapper can be used in your application test code.
 * This is used as concrete implementation of OSWrapper for test.
 */
namespace TestDoubleOSWrapper {

/*!
 * @brief Initialize the TestDoubleOSWrapper
 *
 * This is a helper function for using TestDoubleOSWrapper.
 * All the factories of TestDoubleOSWrapper are generated and resistered.
 *
 * If you want to change a part of the provided test dobule class behavior,
 * you can override the test double class.
 * See test code in TestDoubleOSWrapperTest directory as a sample code.
 */
void init()
{
	static TestDoubleThreadFactory<> theThreadFactory;
	static TestDoubleMutexFactory<> theMutexFactory;
	static TestDoubleEventFlagFactory<> theEventFlagFactory;
	static TestDoubleFixedMemoryPoolFactory<> theFixedMemoryPoolFactory;
	static TestDoubleVariableMemoryPoolFactory<> theVariableMemoryPoolFactory;
	static TestDoublePeriodicTimerFactory<> thePeriodicTimerFactory;
	static TestDoubleOneShotTimerFactory<> theOneShotTimerFactory;

	OSWrapper::registerThreadFactory(&theThreadFactory);
	OSWrapper::registerMutexFactory(&theMutexFactory);
	OSWrapper::registerEventFlagFactory(&theEventFlagFactory);
	OSWrapper::registerFixedMemoryPoolFactory(&theFixedMemoryPoolFactory);
	OSWrapper::registerVariableMemoryPoolFactory(&theVariableMemoryPoolFactory);
	OSWrapper::registerPeriodicTimerFactory(&thePeriodicTimerFactory);
	OSWrapper::registerOneShotTimerFactory(&theOneShotTimerFactory);
}

}

