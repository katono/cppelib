#include "Thread.h"
#include "ThreadFactory.h"
#include "DesignByContract/Assertion.h"

namespace OSWrapper {

const int Thread::INHERIT_PRIORITY = -1;

ThreadFactory* Thread::m_factory = 0;

void Thread::setFactory(ThreadFactory* factory)
{
	m_factory = factory;
}

Thread* Thread::create(Runnable* r, std::size_t stackSize/*= 0U*/, int priority/*= INHERIT_PRIORITY*/, const char* name/*= ""*/)
{
	DBC_PRE(m_factory);
	return m_factory->create(r, stackSize, priority, name);
}

Thread* Thread::create(std::size_t stackSize/*= 0U*/, int priority/*= INHERIT_PRIORITY*/, const char* name/*= ""*/)
{
	DBC_PRE(m_factory);
	return m_factory->create(0, stackSize, priority, name);
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

int Thread::getPriorityMax()
{
	DBC_PRE(m_factory);
	return m_factory->getPriorityMax();
}

int Thread::getPriorityMin()
{
	DBC_PRE(m_factory);
	return m_factory->getPriorityMin();
}

}
