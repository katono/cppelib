#include "MessageQueue.h"
#include "VariableAllocator.h"

namespace OSWrapper {

static VariableAllocator* s_allocator = 0;

void registerMessageQueueAllocator(VariableAllocator* allocator)
{
	s_allocator = allocator;
}

VariableAllocator* getMessageQueueAllocator()
{
	return s_allocator;
}

}
