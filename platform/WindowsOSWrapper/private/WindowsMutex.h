#ifndef WINDOWS_OS_WRAPPER_WINDOWS_MUTEX_H_INCLUDED
#define WINDOWS_OS_WRAPPER_WINDOWS_MUTEX_H_INCLUDED

#include "OSWrapper/Mutex.h"
#include <mutex>

namespace WindowsOSWrapper {

class WindowsMutex : public OSWrapper::Mutex {
private:
	std::timed_mutex m_mutex;

public:
	WindowsMutex();
	virtual ~WindowsMutex();
	virtual OSWrapper::Mutex::Error lock();
	virtual OSWrapper::Mutex::Error tryLock();
	virtual OSWrapper::Mutex::Error tryLockFor(OSWrapper::Timeout tmout);
	virtual OSWrapper::Mutex::Error unlock();
};

}

#endif // WINDOWS_OS_WRAPPER_WINDOWS_MUTEX_H_INCLUDED
