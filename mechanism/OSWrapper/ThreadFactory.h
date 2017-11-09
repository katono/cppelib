#ifndef OS_WRAPPER_THREAD_FACTORY_H_INCLUDED
#define OS_WRAPPER_THREAD_FACTORY_H_INCLUDED

#include <stddef.h>
#include "Thread.h"

namespace OSWrapper {

class Runnable;

class ThreadFactory {
public:
	virtual ~ThreadFactory() {}
	virtual Thread* create(Runnable* r, size_t stackSize, Thread::Priority prio, const char* name) = 0;
	virtual void destroy(Thread* t) = 0;
	virtual void exit() = 0;
	virtual void sleep(unsigned long millis) = 0;
	virtual void yield() = 0;
	virtual Thread* getCurrentThread() = 0;
};

}

#endif // OS_WRAPPER_THREAD_FACTORY_H_INCLUDED
