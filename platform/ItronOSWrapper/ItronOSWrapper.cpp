#include "ItronOSWrapper.h"
#include "ItronThreadFactory.h"
#include "ItronMutexFactory.h"
#include "ItronEventFlagFactory.h"
#include "ItronFixedMemoryPoolFactory.h"
#include "ItronVariableMemoryPoolFactory.h"
#include "ItronPeriodicTimerFactory.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/Mutex.h"
#include "OSWrapper/EventFlag.h"
#include "OSWrapper/FixedMemoryPool.h"
#include "OSWrapper/VariableMemoryPool.h"
#include "OSWrapper/PeriodicTimer.h"

namespace ItronOSWrapper {

void init()
{
	static ItronThreadFactory theThreadFactory;
	static ItronMutexFactory theMutexFactory;
	static ItronEventFlagFactory theEventFlagFactory;
	static ItronFixedMemoryPoolFactory theFixedMemoryPoolFactory;
	static ItronVariableMemoryPoolFactory theVariableMemoryPoolFactory;
	static ItronPeriodicTimerFactory thePeriodicTimerFactory;

	OSWrapper::registerThreadFactory(&theThreadFactory);
	OSWrapper::registerMutexFactory(&theMutexFactory);
	OSWrapper::registerEventFlagFactory(&theEventFlagFactory);
	OSWrapper::registerFixedMemoryPoolFactory(&theFixedMemoryPoolFactory);
	OSWrapper::registerVariableMemoryPoolFactory(&theVariableMemoryPoolFactory);
	OSWrapper::registerPeriodicTimerFactory(&thePeriodicTimerFactory);
}

}

