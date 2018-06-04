#include "WindowsOSWrapperFactory.h"
#include "WindowsThreadFactory.h"
#include "WindowsMutexFactory.h"
#include "WindowsEventFlagFactory.h"
#include "WindowsFixedMemoryPoolFactory.h"
#include "WindowsVariableMemoryPoolFactory.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/Mutex.h"
#include "OSWrapper/EventFlag.h"
#include "OSWrapper/FixedMemoryPool.h"
#include "OSWrapper/VariableMemoryPool.h"

namespace WindowsOSWrapper {

void registerFactory(int lowestPriority, int highestPriority)
{
	static WindowsThreadFactory theThreadFactory(lowestPriority, highestPriority);
	static WindowsMutexFactory theMutexFactory;
	static WindowsEventFlagFactory theEventFlagFactory;
	static WindowsFixedMemoryPoolFactory theFixedMemoryPoolFactory;
	static WindowsVariableMemoryPoolFactory theVariableMemoryPoolFactory;

	OSWrapper::registerThreadFactory(&theThreadFactory);
	OSWrapper::registerMutexFactory(&theMutexFactory);
	OSWrapper::registerEventFlagFactory(&theEventFlagFactory);
	OSWrapper::registerFixedMemoryPoolFactory(&theFixedMemoryPoolFactory);
	OSWrapper::registerVariableMemoryPoolFactory(&theVariableMemoryPoolFactory);
}

}

