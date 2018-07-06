#ifndef OS_WRAPPER_MUTEX_H_INCLUDED
#define OS_WRAPPER_MUTEX_H_INCLUDED

#include "Timeout.h"
#include "OSWrapperError.h"

namespace OSWrapper {

class MutexFactory;

void registerMutexFactory(MutexFactory* factory);

class Mutex {
protected:
	virtual ~Mutex() {}

public:
	static Mutex* create();
	static Mutex* create(int priorityCeiling);
	static void destroy(Mutex* m);

	virtual Error lock() = 0;
	virtual Error tryLock() = 0;
	virtual Error timedLock(Timeout tmout) = 0;
	virtual Error unlock() = 0;

};

struct AdoptLock_t {};
extern const AdoptLock_t AdoptLock;

class LockGuard {
public:
	explicit LockGuard(Mutex* m);
	LockGuard(Mutex* m, AdoptLock_t);
	~LockGuard();

private:
	Mutex* m_mutex;
	Error m_lockErr;

	LockGuard(const LockGuard&);
	LockGuard& operator=(const LockGuard&);
};

}

#endif // OS_WRAPPER_MUTEX_H_INCLUDED
