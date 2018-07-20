#ifndef ITRON_OS_WRAPPER_ITRON_PERIODIC_TIMER_FACTORY_H_INCLUDED
#define ITRON_OS_WRAPPER_ITRON_PERIODIC_TIMER_FACTORY_H_INCLUDED

#include "OSWrapper/PeriodicTimerFactory.h"
#include "kernel.h"

#ifndef MAX_PERIODIC_TIMERS
#define MAX_PERIODIC_TIMERS (256U)
#endif

namespace ItronOSWrapper {

class ItronPeriodicTimerFactory : public OSWrapper::PeriodicTimerFactory {
public:
	ItronPeriodicTimerFactory();
	virtual ~ItronPeriodicTimerFactory();

private:
	virtual OSWrapper::PeriodicTimer* create(OSWrapper::Runnable* r, unsigned long periodInMillis, const char* name);
	virtual void destroy(OSWrapper::PeriodicTimer* t);

	ItronPeriodicTimerFactory(const ItronPeriodicTimerFactory&);
	ItronPeriodicTimerFactory& operator=(const ItronPeriodicTimerFactory&);

	ID m_mpfId;
	ID m_mtxId;
};

}

#endif // ITRON_OS_WRAPPER_ITRON_PERIODIC_TIMER_FACTORY_H_INCLUDED
