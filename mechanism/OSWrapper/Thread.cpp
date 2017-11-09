#include "Thread.h"
#include "ThreadFactory.h"
#include "DesignByContract/Assertion.h"

using namespace OSWrapper;

ThreadFactory* Thread::m_factory = 0;

void Thread::setFactory(ThreadFactory* factory)
{
	m_factory = factory;
}

Thread* Thread::create(Runnable* r, size_t stackSize, Priority prio/*= InheritPriority*/, const char* name/*= ""*/)
{
	DBC_PRE(m_factory);
	return m_factory->create(r, stackSize, prio, name);
}

Thread* Thread::create(size_t stackSize, Priority prio/*= InheritPriority*/, const char* name/*= ""*/)
{
	DBC_PRE(m_factory);
	return m_factory->create(0, stackSize, prio, name);
}

void Thread::destroy(Thread* t)
{
	if ((m_factory != 0) && (t != 0)) {
		m_factory->destroy(t);
	}
}

void Thread::exit()
{
	DBC_PRE(m_factory);
	m_factory->exit();
}

void Thread::sleep(unsigned long millis)
{
	DBC_PRE(m_factory);
	m_factory->sleep(millis);
}

void Thread::yield()
{
	DBC_PRE(m_factory);
	m_factory->yield();
}

Thread* Thread::getCurrentThread()
{
	DBC_PRE(m_factory);
	return m_factory->getCurrentThread();
}

