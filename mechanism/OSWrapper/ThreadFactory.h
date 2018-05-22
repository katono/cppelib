#ifndef OS_WRAPPER_THREAD_FACTORY_H_INCLUDED
#define OS_WRAPPER_THREAD_FACTORY_H_INCLUDED

#include <cstddef>

namespace OSWrapper {

class Runnable;
class Thread;

class ThreadFactory {
public:
	virtual ~ThreadFactory() {}
	virtual Thread* create(Runnable* r, int priority, std::size_t stackSize, void* stackAddress, const char* name) = 0;
	virtual void destroy(Thread* t) = 0;
	virtual void sleep(unsigned long millis) = 0;
	virtual void yield() = 0;
	virtual Thread* getCurrentThread() = 0;
	virtual int getMaxPriority() const = 0;
	virtual int getMinPriority() const = 0;
};

}

#endif // OS_WRAPPER_THREAD_FACTORY_H_INCLUDED
