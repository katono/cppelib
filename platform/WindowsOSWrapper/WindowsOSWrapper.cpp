#include "WindowsOSWrapper.h"
#include "WindowsThreadFactory.h"
#include "WindowsMutexFactory.h"
#include "WindowsEventFlagFactory.h"
#include "WindowsFixedMemoryPoolFactory.h"
#include "WindowsVariableMemoryPoolFactory.h"
#include "WindowsPeriodicTimerFactory.h"
#include "WindowsOneShotTimerFactory.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/Mutex.h"
#include "OSWrapper/EventFlag.h"
#include "OSWrapper/FixedMemoryPool.h"
#include "OSWrapper/VariableMemoryPool.h"
#include "OSWrapper/PeriodicTimer.h"
#include "OSWrapper/OneShotTimer.h"

/*!
 * @brief Implementation of OSWrapper for Windows platform
 */
namespace WindowsOSWrapper {

/*!
 * @brief Initialize the WindowsOSWrapper
 * @param lowestPriority the lowest priority of OSWrapper::Thread
 * @param highestPriority the highest priority of OSWrapper::Thread
 *
 * This is a helper function for using WindowsOSWrapper.
 * All the factories of WindowsOSWrapper are generated and resistered.
 * If you use WindowsOSWrapper, call this function when the application is initialized.
 * You don't have to register for each factory.
 */
void init(int lowestPriority, int highestPriority)
{
	static WindowsThreadFactory theThreadFactory(lowestPriority, highestPriority);
	static WindowsMutexFactory theMutexFactory;
	static WindowsEventFlagFactory theEventFlagFactory;
	static WindowsFixedMemoryPoolFactory theFixedMemoryPoolFactory;
	static WindowsVariableMemoryPoolFactory theVariableMemoryPoolFactory;
	static WindowsPeriodicTimerFactory thePeriodicTimerFactory;
	static WindowsOneShotTimerFactory theOneShotTimerFactory;

	OSWrapper::registerThreadFactory(&theThreadFactory);
	OSWrapper::registerMutexFactory(&theMutexFactory);
	OSWrapper::registerEventFlagFactory(&theEventFlagFactory);
	OSWrapper::registerFixedMemoryPoolFactory(&theFixedMemoryPoolFactory);
	OSWrapper::registerVariableMemoryPoolFactory(&theVariableMemoryPoolFactory);
	OSWrapper::registerPeriodicTimerFactory(&thePeriodicTimerFactory);
	OSWrapper::registerOneShotTimerFactory(&theOneShotTimerFactory);
}

}

