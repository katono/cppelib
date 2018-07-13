#ifndef OS_WRAPPER_PERIODIC_TIMER_FACTORY_H_INCLUDED
#define OS_WRAPPER_PERIODIC_TIMER_FACTORY_H_INCLUDED

namespace OSWrapper {

class Runnable;
class PeriodicTimer;

class PeriodicTimerFactory {
public:
	virtual ~PeriodicTimerFactory() {}
	virtual PeriodicTimer* create(Runnable* r, unsigned long periodInMillis, const char* name) = 0;
	virtual void destroy(PeriodicTimer* t) = 0;
};

}

#endif // OS_WRAPPER_PERIODIC_TIMER_FACTORY_H_INCLUDED
