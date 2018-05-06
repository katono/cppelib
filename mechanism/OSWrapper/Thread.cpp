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


Thread::ExceptionHandler* Thread::m_defaultExceptionHandler = 0;

void Thread::setDefaultExceptionHandler(Thread::ExceptionHandler* handler)
{
	m_defaultExceptionHandler = handler;
}

Thread::ExceptionHandler* Thread::getDefaultExceptionHandler()
{
	return m_defaultExceptionHandler;
}

void Thread::setExceptionHandler(Thread::ExceptionHandler* handler)
{
	m_exceptionHandler = handler;
}

Thread::ExceptionHandler* Thread::getExceptionHandler() const
{
	return m_exceptionHandler;
}

void Thread::handleException(const std::exception& e)
{
	Thread* t = Thread::getCurrentThread();
	if (m_exceptionHandler != 0) {
		m_exceptionHandler->handle(t, e);
	} else if (m_defaultExceptionHandler != 0) {
		m_defaultExceptionHandler->handle(t, e);
	}
}

class UnknownException : public std::exception {
public:
	explicit UnknownException(const char* msg) : m_msg(msg) {}
	const char* what() const throw()
	{
		return m_msg;
	}
private:
	const char* m_msg;
};

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
	catch (...) {
		handleException(UnknownException("Unknown Exception"));
	}
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
	throw Exit();
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

int Thread::getNormalPriority()
{
	CHECK_PRECOND(s_factory);
	return (s_factory->getMinPriority() + s_factory->getMaxPriority()) / 2;
}

}
