#ifndef STDCPP_OS_WRAPPER_STDCPP_PERIODIC_TIMER_FACTORY_H_INCLUDED
#define STDCPP_OS_WRAPPER_STDCPP_PERIODIC_TIMER_FACTORY_H_INCLUDED

#include "OSWrapper/PeriodicTimerFactory.h"
#include <mutex>

namespace StdCppOSWrapper {

class StdCppPeriodicTimerFactory : public OSWrapper::PeriodicTimerFactory {
public:
	StdCppPeriodicTimerFactory();
	virtual ~StdCppPeriodicTimerFactory() {}

private:
	virtual OSWrapper::PeriodicTimer* create(OSWrapper::Runnable* r, unsigned long periodInMillis, const char* name);
	virtual void destroy(OSWrapper::PeriodicTimer* t);

	StdCppPeriodicTimerFactory(const StdCppPeriodicTimerFactory&);
	StdCppPeriodicTimerFactory& operator=(const StdCppPeriodicTimerFactory&);

	std::mutex m_mutex;
};

}

#endif // STDCPP_OS_WRAPPER_STDCPP_PERIODIC_TIMER_FACTORY_H_INCLUDED
