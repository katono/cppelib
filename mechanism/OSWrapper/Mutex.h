#ifndef OS_WRAPPER_MUTEX_H_INCLUDED
#define OS_WRAPPER_MUTEX_H_INCLUDED

#include "Timeout.h"

namespace OSWrapper {

class MutexFactory;

class Mutex {
public:
	enum Error {
		OK,
		TimedOut,
		CalledByNonThread,
		LockedRecursively,
		NotLocked,
		UnknownError
	};
	virtual ~Mutex() {}

	static void setFactory(MutexFactory* factory);

	static Mutex* create();
	static Mutex* create(int priorityCeiling);
	static void destroy(Mutex* m);

	virtual Error lock() = 0;
	virtual Error tryLock() = 0;
	virtual Error tryLockFor(Timeout tmout) = 0;
	virtual Error unlock() = 0;

private:
	static MutexFactory* m_factory;
};

class LockGuard {
public:
	explicit LockGuard(Mutex* m);
	~LockGuard();

private:
	Mutex* m_mutex;

	LockGuard(const LockGuard&);
	LockGuard& operator=(const LockGuard&);
};

}

#endif // OS_WRAPPER_MUTEX_H_INCLUDED
