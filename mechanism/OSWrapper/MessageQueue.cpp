#include "MessageQueue.h"
#include "VariableAllocator.h"

namespace OSWrapper {

static VariableAllocator* mqAllocator = 0;

void registerMessageQueueAllocator(VariableAllocator* allocator)
{
	mqAllocator = allocator;
}

VariableAllocator* getMessageQueueAllocator()
{
	return mqAllocator;
}

}
