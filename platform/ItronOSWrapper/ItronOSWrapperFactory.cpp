#include "ItronOSWrapperFactory.h"
#include "ItronThreadFactory.h"
#include "ItronMutexFactory.h"
#include "ItronEventFlagFactory.h"
#include "ItronFixedMemoryPoolFactory.h"
#include "ItronVariableMemoryPoolFactory.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/Mutex.h"
#include "OSWrapper/EventFlag.h"
#include "OSWrapper/FixedMemoryPool.h"
#include "OSWrapper/VariableMemoryPool.h"

namespace ItronOSWrapper {

void registerFactory()
{
	static ItronThreadFactory theThreadFactory;
	static ItronMutexFactory theMutexFactory;
	static ItronEventFlagFactory theEventFlagFactory;
	static ItronFixedMemoryPoolFactory theFixedMemoryPoolFactory;
	static ItronVariableMemoryPoolFactory theVariableMemoryPoolFactory;

	OSWrapper::registerThreadFactory(&theThreadFactory);
	OSWrapper::registerMutexFactory(&theMutexFactory);
	OSWrapper::registerEventFlagFactory(&theEventFlagFactory);
	OSWrapper::registerFixedMemoryPoolFactory(&theFixedMemoryPoolFactory);
	OSWrapper::registerVariableMemoryPoolFactory(&theVariableMemoryPoolFactory);
}

}

