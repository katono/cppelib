#ifndef POSIX_OS_WRAPPER_POSIX_PERIODIC_TIMER_FACTORY_H_INCLUDED
#define POSIX_OS_WRAPPER_POSIX_PERIODIC_TIMER_FACTORY_H_INCLUDED

#include "OSWrapper/PeriodicTimerFactory.h"
#include <mutex>

namespace PosixOSWrapper {

class PosixPeriodicTimerFactory : public OSWrapper::PeriodicTimerFactory {
public:
	PosixPeriodicTimerFactory();
	virtual ~PosixPeriodicTimerFactory() {}

private:
	virtual OSWrapper::PeriodicTimer* create(OSWrapper::Runnable* r, unsigned long periodInMillis, const char* name);
	virtual void destroy(OSWrapper::PeriodicTimer* t);

	PosixPeriodicTimerFactory(const PosixPeriodicTimerFactory&);
	PosixPeriodicTimerFactory& operator=(const PosixPeriodicTimerFactory&);

	std::mutex m_mutex;
};

}

#endif // POSIX_OS_WRAPPER_POSIX_PERIODIC_TIMER_FACTORY_H_INCLUDED
