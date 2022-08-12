#include "PeriodicTimer.h"
#include "PeriodicTimerFactory.h"
#include "Runnable.h"
#include "Assertion/Assertion.h"
#include <exception>

namespace OSWrapper {

static PeriodicTimerFactory* s_factory = 0;

void registerPeriodicTimerFactory(PeriodicTimerFactory* factory)
{
	s_factory = factory;
}


PeriodicTimer::UncaughtExceptionHandler* PeriodicTimer::m_defaultUncaughtExceptionHandler = 0;

void PeriodicTimer::setDefaultUncaughtExceptionHandler(PeriodicTimer::UncaughtExceptionHandler* handler)
{
	m_defaultUncaughtExceptionHandler = handler;
}

PeriodicTimer::UncaughtExceptionHandler* PeriodicTimer::getDefaultUncaughtExceptionHandler()
{
	return m_defaultUncaughtExceptionHandler;
}

void PeriodicTimer::setUncaughtExceptionHandler(PeriodicTimer::UncaughtExceptionHandler* handler)
{
	m_uncaughtExceptionHandler = handler;
}

PeriodicTimer::UncaughtExceptionHandler* PeriodicTimer::getUncaughtExceptionHandler() const
{
	return m_uncaughtExceptionHandler;
}

void PeriodicTimer::handleException(const char* msg)
{
#ifndef CPPELIB_NO_EXCEPTIONS
	try {
		stop();
		if (m_uncaughtExceptionHandler != 0) {
			m_uncaughtExceptionHandler->handle(this, msg);
		} else if (m_defaultUncaughtExceptionHandler != 0) {
			m_defaultUncaughtExceptionHandler->handle(this, msg);
		}
	}
	catch (...) {
		// ignore exception
	}
#else
	(void)msg;
#endif
}

/*!
 * @brief Common timer main method
 * @note Called in the concrete class derived from PeriodicTimer.
 */
void PeriodicTimer::timerMain()
{
#ifndef CPPELIB_NO_EXCEPTIONS
	try {
#endif
		if (m_runnable != 0) {
			m_runnable->run();
		}
#ifndef CPPELIB_NO_EXCEPTIONS
	}
	catch (const std::exception& e) {
		handleException(e.what());
	}
	catch (const Assertion::Failure& e) {
		handleException(e.message());
	}
	catch (...) {
		handleException("Unknown Exception");
	}
#endif
}

PeriodicTimer* PeriodicTimer::create(Runnable* r, unsigned long periodInMillis, const char* name/*= ""*/)
{
	CHECK_ASSERT(s_factory);
	if (r == 0) {
		return 0;
	}
	if (periodInMillis == 0U) {
		return 0;
	}
	return s_factory->create(r, periodInMillis, name);
}

void PeriodicTimer::destroy(PeriodicTimer* t)
{
	if ((s_factory != 0) && (t != 0)) {
		s_factory->destroy(t);
	}
}

}
