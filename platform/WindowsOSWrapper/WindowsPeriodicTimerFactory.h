#ifndef WINDOWS_OS_WRAPPER_WINDOWS_PERIODIC_TIMER_FACTORY_H_INCLUDED
#define WINDOWS_OS_WRAPPER_WINDOWS_PERIODIC_TIMER_FACTORY_H_INCLUDED

#include "OSWrapper/PeriodicTimerFactory.h"
#include <mutex>

namespace WindowsOSWrapper {

class WindowsPeriodicTimerFactory : public OSWrapper::PeriodicTimerFactory {
public:
	WindowsPeriodicTimerFactory();
	virtual ~WindowsPeriodicTimerFactory() {}

private:
	virtual OSWrapper::PeriodicTimer* create(OSWrapper::Runnable* r, unsigned long periodInMillis, const char* name);
	virtual void destroy(OSWrapper::PeriodicTimer* t);

	WindowsPeriodicTimerFactory(const WindowsPeriodicTimerFactory&);
	WindowsPeriodicTimerFactory& operator=(const WindowsPeriodicTimerFactory&);

	std::mutex m_mutex;
};

}

#endif // WINDOWS_OS_WRAPPER_WINDOWS_PERIODIC_TIMER_FACTORY_H_INCLUDED
