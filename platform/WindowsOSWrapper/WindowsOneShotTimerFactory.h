#ifndef WINDOWS_OS_WRAPPER_WINDOWS_ONESHOT_TIMER_FACTORY_H_INCLUDED
#define WINDOWS_OS_WRAPPER_WINDOWS_ONESHOT_TIMER_FACTORY_H_INCLUDED

#include "OSWrapper/OneShotTimerFactory.h"
#include <mutex>

namespace WindowsOSWrapper {

class WindowsOneShotTimerFactory : public OSWrapper::OneShotTimerFactory {
public:
	WindowsOneShotTimerFactory();
	virtual ~WindowsOneShotTimerFactory() {}

private:
	virtual OSWrapper::OneShotTimer* create(OSWrapper::Runnable* r, const char* name);
	virtual void destroy(OSWrapper::OneShotTimer* t);

	WindowsOneShotTimerFactory(const WindowsOneShotTimerFactory&);
	WindowsOneShotTimerFactory& operator=(const WindowsOneShotTimerFactory&);

	std::mutex m_mutex;
};

}

#endif // WINDOWS_OS_WRAPPER_WINDOWS_ONESHOT_TIMER_FACTORY_H_INCLUDED
