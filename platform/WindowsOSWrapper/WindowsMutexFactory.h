#ifndef WINDOWS_OS_WRAPPER_WINDOWS_MUTEX_FACTORY_H_INCLUDED
#define WINDOWS_OS_WRAPPER_WINDOWS_MUTEX_FACTORY_H_INCLUDED

#include "OSWrapper/MutexFactory.h"
#include <mutex>

namespace WindowsOSWrapper {

class WindowsMutexFactory : public OSWrapper::MutexFactory {
public:
	WindowsMutexFactory();
	virtual ~WindowsMutexFactory() {}

private:
	virtual OSWrapper::Mutex* create();
	virtual OSWrapper::Mutex* create(int priorityCeiling);
	virtual void destroy(OSWrapper::Mutex* m);

	std::mutex m_mutex;
};

}

#endif // WINDOWS_OS_WRAPPER_WINDOWS_MUTEX_FACTORY_H_INCLUDED
