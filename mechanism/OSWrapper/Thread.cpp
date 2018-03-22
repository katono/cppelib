#include "Thread.h"
#include "ThreadFactory.h"
#include "DesignByContract/Assertion.h"

namespace OSWrapper {

const int Thread::INHERIT_PRIORITY = -1;

static ThreadFactory* s_factory = 0;

void registerThreadFactory(ThreadFactory* factory)
{
	s_factory = factory;
}

Thread* Thread::create(Runnable* r, std::size_t stackSize/*= 0U*/, int priority/*= INHERIT_PRIORITY*/, const char* name/*= ""*/)
{
	DBC_PRE(s_factory);
	if (r == 0) {
		return 0;
	}
	return s_factory->create(r, stackSize, priority, name);
}

void Thread::destroy(Thread* t)
{
	if ((s_factory != 0) && (t != 0)) {
		s_factory->destroy(t);
	}
}

void Thread::exit()
{
	DBC_PRE(s_factory);
	s_factory->exit();
}

void Thread::sleep(unsigned long millis)
{
	DBC_PRE(s_factory);
	s_factory->sleep(millis);
}

void Thread::yield()
{
	DBC_PRE(s_factory);
	s_factory->yield();
}

Thread* Thread::getCurrentThread()
{
	DBC_PRE(s_factory);
	return s_factory->getCurrentThread();
}

int Thread::getPriorityMax()
{
	DBC_PRE(s_factory);
	return s_factory->getPriorityMax();
}

int Thread::getPriorityMin()
{
	DBC_PRE(s_factory);
	return s_factory->getPriorityMin();
}

}
