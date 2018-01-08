#ifndef OS_HELPER_LOCK_GUARD_H_INCLUDED
#define OS_HELPER_LOCK_GUARD_H_INCLUDED

#include "OSWrapper/Mutex.h"

namespace OSHelper {

class LockGuard {
public:
	explicit LockGuard(OSWrapper::Mutex* m)
	: m_mutex(m)
	{
		if (m_mutex != 0) {
			m_mutex->lock();
		}
	}

	~LockGuard()
	{
		if (m_mutex != 0) {
			m_mutex->unlock();
		}
	}
private:
	OSWrapper::Mutex* m_mutex;

	LockGuard(const LockGuard&);
	LockGuard& operator=(const LockGuard&);
};

}

#endif // OS_HELPER_LOCK_GUARD_H_INCLUDED
