#include "StdCppOSWrapper.h"
#include "StdCppThreadFactory.h"
#include "StdCppMutexFactory.h"
#include "StdCppEventFlagFactory.h"
#include "StdCppFixedMemoryPoolFactory.h"
#include "StdCppVariableMemoryPoolFactory.h"
#include "StdCppPeriodicTimerFactory.h"
#include "StdCppOneShotTimerFactory.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/Mutex.h"
#include "OSWrapper/EventFlag.h"
#include "OSWrapper/FixedMemoryPool.h"
#include "OSWrapper/VariableMemoryPool.h"
#include "OSWrapper/PeriodicTimer.h"
#include "OSWrapper/OneShotTimer.h"

/*!
 * @brief Implementation of OSWrapper for standard C++11 using std::thread
 */
namespace StdCppOSWrapper {

/*!
 * @brief Initialize the StdCppOSWrapper
 *
 * This is a helper function for using StdCppOSWrapper.
 * All the factories of StdCppOSWrapper are generated and resistered.
 * If you use StdCppOSWrapper, call this function when the application is initialized.
 * You don't have to register for each factory.
 */
void init()
{
	static StdCppThreadFactory theThreadFactory;
	static StdCppMutexFactory theMutexFactory;
	static StdCppEventFlagFactory theEventFlagFactory;
	static StdCppFixedMemoryPoolFactory theFixedMemoryPoolFactory;
	static StdCppVariableMemoryPoolFactory theVariableMemoryPoolFactory;
	static StdCppPeriodicTimerFactory thePeriodicTimerFactory;
	static StdCppOneShotTimerFactory theOneShotTimerFactory;

	OSWrapper::registerThreadFactory(&theThreadFactory);
	OSWrapper::registerMutexFactory(&theMutexFactory);
	OSWrapper::registerEventFlagFactory(&theEventFlagFactory);
	OSWrapper::registerFixedMemoryPoolFactory(&theFixedMemoryPoolFactory);
	OSWrapper::registerVariableMemoryPoolFactory(&theVariableMemoryPoolFactory);
	OSWrapper::registerPeriodicTimerFactory(&thePeriodicTimerFactory);
	OSWrapper::registerOneShotTimerFactory(&theOneShotTimerFactory);
}

}

