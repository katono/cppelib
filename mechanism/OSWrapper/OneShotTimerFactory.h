#ifndef OS_WRAPPER_ONESHOT_TIMER_FACTORY_H_INCLUDED
#define OS_WRAPPER_ONESHOT_TIMER_FACTORY_H_INCLUDED

namespace OSWrapper {

class Runnable;
class OneShotTimer;

class OneShotTimerFactory {
public:
	virtual ~OneShotTimerFactory() {}
	virtual OneShotTimer* create(Runnable* r, const char* name) = 0;
	virtual void destroy(OneShotTimer* t) = 0;
};

}

#endif // OS_WRAPPER_ONESHOT_TIMER_FACTORY_H_INCLUDED
