#include "StdCppOSWrapper.h"
#include "StdCppThreadFactory.h"
#include "StdCppMutexFactory.h"
#include "StdCppEventFlagFactory.h"
#include "StdCppFixedMemoryPoolFactory.h"
#include "StdCppVariableMemoryPoolFactory.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/Mutex.h"
#include "OSWrapper/EventFlag.h"
#include "OSWrapper/FixedMemoryPool.h"
#include "OSWrapper/VariableMemoryPool.h"

namespace StdCppOSWrapper {

void init()
{
	static StdCppThreadFactory theThreadFactory;
	static StdCppMutexFactory theMutexFactory;
	static StdCppEventFlagFactory theEventFlagFactory;
	static StdCppFixedMemoryPoolFactory theFixedMemoryPoolFactory;
	static StdCppVariableMemoryPoolFactory theVariableMemoryPoolFactory;

	OSWrapper::registerThreadFactory(&theThreadFactory);
	OSWrapper::registerMutexFactory(&theMutexFactory);
	OSWrapper::registerEventFlagFactory(&theEventFlagFactory);
	OSWrapper::registerFixedMemoryPoolFactory(&theFixedMemoryPoolFactory);
	OSWrapper::registerVariableMemoryPoolFactory(&theVariableMemoryPoolFactory);
}

}

