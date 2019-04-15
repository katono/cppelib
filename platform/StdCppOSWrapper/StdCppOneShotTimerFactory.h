#ifndef STDCPP_OS_WRAPPER_STDCPP_ONESHOT_TIMER_FACTORY_H_INCLUDED
#define STDCPP_OS_WRAPPER_STDCPP_ONESHOT_TIMER_FACTORY_H_INCLUDED

#include "OSWrapper/OneShotTimerFactory.h"
#include <mutex>

namespace StdCppOSWrapper {

class StdCppOneShotTimerFactory : public OSWrapper::OneShotTimerFactory {
public:
	StdCppOneShotTimerFactory();
	virtual ~StdCppOneShotTimerFactory() {}

private:
	virtual OSWrapper::OneShotTimer* create(OSWrapper::Runnable* r, const char* name);
	virtual void destroy(OSWrapper::OneShotTimer* t);

	StdCppOneShotTimerFactory(const StdCppOneShotTimerFactory&);
	StdCppOneShotTimerFactory& operator=(const StdCppOneShotTimerFactory&);

	std::mutex m_mutex;
};

}

#endif // STDCPP_OS_WRAPPER_STDCPP_ONESHOT_TIMER_FACTORY_H_INCLUDED
