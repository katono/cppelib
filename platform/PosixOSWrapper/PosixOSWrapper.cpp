#include "PosixOSWrapper.h"
#include "PosixThreadFactory.h"
#include "PosixMutexFactory.h"
#include "PosixEventFlagFactory.h"
#include "PosixFixedMemoryPoolFactory.h"
#include "PosixVariableMemoryPoolFactory.h"
#include "PosixPeriodicTimerFactory.h"
#include "PosixOneShotTimerFactory.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/Mutex.h"
#include "OSWrapper/EventFlag.h"
#include "OSWrapper/FixedMemoryPool.h"
#include "OSWrapper/VariableMemoryPool.h"
#include "OSWrapper/PeriodicTimer.h"
#include "OSWrapper/OneShotTimer.h"

/*!
 * @brief Implementation of OSWrapper for POSIX platform
 */
namespace PosixOSWrapper {

/*!
 * @brief Initialize the PosixOSWrapper
 * @param lowestPriority the lowest priority of OSWrapper::Thread
 * @param highestPriority the highest priority of OSWrapper::Thread
 *
 * This is a helper function for using PosixOSWrapper.
 * All the factories of PosixOSWrapper are generated and resistered.
 * If you use PosixOSWrapper, call this function when the application is initialized.
 * You don't have to register for each factory.
 */
void init(int lowestPriority, int highestPriority)
{
	static PosixThreadFactory theThreadFactory(lowestPriority, highestPriority);
	static PosixMutexFactory theMutexFactory;
	static PosixEventFlagFactory theEventFlagFactory;
	static PosixFixedMemoryPoolFactory theFixedMemoryPoolFactory;
	static PosixVariableMemoryPoolFactory theVariableMemoryPoolFactory;
	static PosixPeriodicTimerFactory thePeriodicTimerFactory;
	static PosixOneShotTimerFactory theOneShotTimerFactory;

	OSWrapper::registerThreadFactory(&theThreadFactory);
	OSWrapper::registerMutexFactory(&theMutexFactory);
	OSWrapper::registerEventFlagFactory(&theEventFlagFactory);
	OSWrapper::registerFixedMemoryPoolFactory(&theFixedMemoryPoolFactory);
	OSWrapper::registerVariableMemoryPoolFactory(&theVariableMemoryPoolFactory);
	OSWrapper::registerPeriodicTimerFactory(&thePeriodicTimerFactory);
	OSWrapper::registerOneShotTimerFactory(&theOneShotTimerFactory);
}

}

