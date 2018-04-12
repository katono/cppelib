#ifndef OS_WRAPPER_THREAD_H_INCLUDED
#define OS_WRAPPER_THREAD_H_INCLUDED

#include <cstddef>
#include "Timeout.h"
#include "OSWrapperError.h"

namespace OSWrapper {

class Runnable;
class ThreadFactory;

void registerThreadFactory(ThreadFactory* factory);

class Thread {
public:
	static const int INHERIT_PRIORITY;

	virtual ~Thread() {}

	static Thread* create(Runnable* r, std::size_t stackSize = 0U, int priority = INHERIT_PRIORITY, const char* name = "");
	static void destroy(Thread* t);
	static void exit();
	static void sleep(unsigned long millis);
	static void yield();
	static Thread* getCurrentThread();
	static int getHighestPriority();
	static int getLowestPriority();

	virtual void start() = 0;
	virtual Error wait() = 0;
	virtual Error tryWait() = 0;
	virtual Error timedWait(Timeout tmout) = 0;
	virtual bool isFinished() const = 0;

	virtual void setName(const char* name) = 0;
	virtual const char* getName() const = 0;
	virtual void setPriority(int priority) = 0;
	virtual int getPriority() const = 0;
	virtual std::size_t getStackSize() const = 0;
	virtual void* getNativeHandle() = 0;

};

}

#endif // OS_WRAPPER_THREAD_H_INCLUDED
