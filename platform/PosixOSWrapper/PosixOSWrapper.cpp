#include "PosixOSWrapper.h"
#include "PosixThreadFactory.h"
#include "PosixMutexFactory.h"
#include "PosixEventFlagFactory.h"
#include "PosixFixedMemoryPoolFactory.h"
#include "PosixVariableMemoryPoolFactory.h"
#include "PosixPeriodicTimerFactory.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/Mutex.h"
#include "OSWrapper/EventFlag.h"
#include "OSWrapper/FixedMemoryPool.h"
#include "OSWrapper/VariableMemoryPool.h"
#include "OSWrapper/PeriodicTimer.h"

namespace PosixOSWrapper {

void init(int lowestPriority, int highestPriority)
{
	static PosixThreadFactory theThreadFactory(lowestPriority, highestPriority);
	static PosixMutexFactory theMutexFactory;
	static PosixEventFlagFactory theEventFlagFactory;
	static PosixFixedMemoryPoolFactory theFixedMemoryPoolFactory;
	static PosixVariableMemoryPoolFactory theVariableMemoryPoolFactory;
	static PosixPeriodicTimerFactory thePeriodicTimerFactory;

	OSWrapper::registerThreadFactory(&theThreadFactory);
	OSWrapper::registerMutexFactory(&theMutexFactory);
	OSWrapper::registerEventFlagFactory(&theEventFlagFactory);
	OSWrapper::registerFixedMemoryPoolFactory(&theFixedMemoryPoolFactory);
	OSWrapper::registerVariableMemoryPoolFactory(&theVariableMemoryPoolFactory);
	OSWrapper::registerPeriodicTimerFactory(&thePeriodicTimerFactory);
}

}

