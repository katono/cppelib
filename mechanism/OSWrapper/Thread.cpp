#include "Thread.h"
#include "ThreadFactory.h"
#include "Runnable.h"
#include "Assertion/Assertion.h"

namespace OSWrapper {

const int Thread::INHERIT_PRIORITY = -1;

static ThreadFactory* s_factory = 0;

void registerThreadFactory(ThreadFactory* factory)
{
	s_factory = factory;
}


Thread::UncaughtExceptionHandler* Thread::m_defaultUncaughtExceptionHandler = 0;

void Thread::setDefaultUncaughtExceptionHandler(Thread::UncaughtExceptionHandler* handler)
{
	m_defaultUncaughtExceptionHandler = handler;
}

Thread::UncaughtExceptionHandler* Thread::getDefaultUncaughtExceptionHandler()
{
	return m_defaultUncaughtExceptionHandler;
}

void Thread::setUncaughtExceptionHandler(Thread::UncaughtExceptionHandler* handler)
{
	m_uncaughtExceptionHandler = handler;
}

Thread::UncaughtExceptionHandler* Thread::getUncaughtExceptionHandler() const
{
	return m_uncaughtExceptionHandler;
}

void Thread::handleException(const std::exception& e)
{
	if (m_uncaughtExceptionHandler != 0) {
		m_uncaughtExceptionHandler->handle(this, e);
	} else if (m_defaultUncaughtExceptionHandler != 0) {
		m_defaultUncaughtExceptionHandler->handle(this, e);
	}
}

void Thread::threadMain()
{
	try {
		if (m_runnable != 0) {
			m_runnable->run();
		}
	}
	catch (const Exit&) {
		// do nothing
	}
	catch (const std::exception& e) {
		handleException(e);
	}
	catch (const Assertion::Error& e) {
		handleException(OtherException(e.message()));
	}
	catch (...) {
		handleException(OtherException("Unknown Exception"));
	}
}

Thread* Thread::create(Runnable* r, int priority/*= INHERIT_PRIORITY*/, std::size_t stackSize/*= 0U*/, void* stackAddress/*= 0*/, const char* name/*= ""*/)
{
	CHECK_ASSERT(s_factory);
	if (r == 0) {
		return 0;
	}
	return s_factory->create(r, priority, stackSize, stackAddress, name);
}

void Thread::destroy(Thread* t)
{
	if ((s_factory != 0) && (t != 0)) {
		s_factory->destroy(t);
	}
}

void Thread::exit()
{
	throw Exit();
}

void Thread::sleep(unsigned long millis)
{
	CHECK_ASSERT(s_factory);
	s_factory->sleep(millis);
}

void Thread::yield()
{
	CHECK_ASSERT(s_factory);
	s_factory->yield();
}

Thread* Thread::getCurrentThread()
{
	CHECK_ASSERT(s_factory);
	return s_factory->getCurrentThread();
}

int Thread::getMaxPriority()
{
	CHECK_ASSERT(s_factory);
	return s_factory->getMaxPriority();
}

int Thread::getMinPriority()
{
	CHECK_ASSERT(s_factory);
	return s_factory->getMinPriority();
}

int Thread::getNormalPriority()
{
	CHECK_ASSERT(s_factory);
	return (s_factory->getMinPriority() + s_factory->getMaxPriority()) / 2;
}

}
