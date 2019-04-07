#include "WindowsOSWrapper.h"
#include "WindowsThreadFactory.h"
#include "WindowsMutexFactory.h"
#include "WindowsEventFlagFactory.h"
#include "WindowsFixedMemoryPoolFactory.h"
#include "WindowsVariableMemoryPoolFactory.h"
#include "WindowsPeriodicTimerFactory.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/Mutex.h"
#include "OSWrapper/EventFlag.h"
#include "OSWrapper/FixedMemoryPool.h"
#include "OSWrapper/VariableMemoryPool.h"
#include "OSWrapper/PeriodicTimer.h"

namespace WindowsOSWrapper {

void init(int lowestPriority, int highestPriority)
{
	static WindowsThreadFactory theThreadFactory(lowestPriority, highestPriority);
	static WindowsMutexFactory theMutexFactory;
	static WindowsEventFlagFactory theEventFlagFactory;
	static WindowsFixedMemoryPoolFactory theFixedMemoryPoolFactory;
	static WindowsVariableMemoryPoolFactory theVariableMemoryPoolFactory;
	static WindowsPeriodicTimerFactory thePeriodicTimerFactory;

	OSWrapper::registerThreadFactory(&theThreadFactory);
	OSWrapper::registerMutexFactory(&theMutexFactory);
	OSWrapper::registerEventFlagFactory(&theEventFlagFactory);
	OSWrapper::registerFixedMemoryPoolFactory(&theFixedMemoryPoolFactory);
	OSWrapper::registerVariableMemoryPoolFactory(&theVariableMemoryPoolFactory);
	OSWrapper::registerPeriodicTimerFactory(&thePeriodicTimerFactory);
}

}

