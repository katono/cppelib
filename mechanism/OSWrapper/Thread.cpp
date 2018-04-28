#include "Thread.h"
#include "ThreadFactory.h"
#include "Assertion/Assertion.h"

namespace OSWrapper {

const int Thread::INHERIT_PRIORITY = -1;

static ThreadFactory* s_factory = 0;

void registerThreadFactory(ThreadFactory* factory)
{
	s_factory = factory;
}

Thread* Thread::create(Runnable* r, std::size_t stackSize/*= 0U*/, int priority/*= INHERIT_PRIORITY*/, const char* name/*= ""*/)
{
	CHECK_PRECOND(s_factory);
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
	CHECK_PRECOND(s_factory);
	s_factory->exit();
}

void Thread::sleep(unsigned long millis)
{
	CHECK_PRECOND(s_factory);
	s_factory->sleep(millis);
}

void Thread::yield()
{
	CHECK_PRECOND(s_factory);
	s_factory->yield();
}

Thread* Thread::getCurrentThread()
{
	CHECK_PRECOND(s_factory);
	return s_factory->getCurrentThread();
}

int Thread::getMaxPriority()
{
	CHECK_PRECOND(s_factory);
	return s_factory->getMaxPriority();
}

int Thread::getMinPriority()
{
	CHECK_PRECOND(s_factory);
	return s_factory->getMinPriority();
}

}
