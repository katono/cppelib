#include "OneShotTimer.h"
#include "OneShotTimerFactory.h"
#include "Runnable.h"
#include "Assertion/Assertion.h"
#include <exception>

namespace OSWrapper {

static OneShotTimerFactory* s_factory = 0;

void registerOneShotTimerFactory(OneShotTimerFactory* factory)
{
	s_factory = factory;
}


OneShotTimer::UncaughtExceptionHandler* OneShotTimer::m_defaultUncaughtExceptionHandler = 0;

void OneShotTimer::setDefaultUncaughtExceptionHandler(OneShotTimer::UncaughtExceptionHandler* handler)
{
	m_defaultUncaughtExceptionHandler = handler;
}

OneShotTimer::UncaughtExceptionHandler* OneShotTimer::getDefaultUncaughtExceptionHandler()
{
	return m_defaultUncaughtExceptionHandler;
}

void OneShotTimer::setUncaughtExceptionHandler(OneShotTimer::UncaughtExceptionHandler* handler)
{
	m_uncaughtExceptionHandler = handler;
}

OneShotTimer::UncaughtExceptionHandler* OneShotTimer::getUncaughtExceptionHandler() const
{
	return m_uncaughtExceptionHandler;
}

void OneShotTimer::handleException(const char* msg)
{
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
}

void OneShotTimer::timerMain()
{
	try {
		if (m_runnable != 0) {
			m_runnable->run();
		}
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
}

OneShotTimer* OneShotTimer::create(Runnable* r, const char* name/*= ""*/)
{
	CHECK_ASSERT(s_factory);
	if (r == 0) {
		return 0;
	}
	return s_factory->create(r, name);
}

void OneShotTimer::destroy(OneShotTimer* t)
{
	if ((s_factory != 0) && (t != 0)) {
		s_factory->destroy(t);
	}
}

}
