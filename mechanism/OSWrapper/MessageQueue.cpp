#include "MessageQueue.h"
#include "VariableMemoryPool.h"

namespace OSWrapper {

static VariableMemoryPool* s_pool = 0;

void registerMessageQueueMemoryPool(VariableMemoryPool* pool)
{
	s_pool = pool;
}

VariableMemoryPool* getMessageQueueMemoryPool()
{
	return s_pool;
}

}
