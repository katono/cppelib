#ifndef OS_WRAPPER_THREAD_H_INCLUDED
#define OS_WRAPPER_THREAD_H_INCLUDED

#include <stddef.h>
#include "Runnable.h"

namespace OSWrapper {

class ThreadFactory;

class Thread {
public:
	enum Priority {
		IdlePriority,
		LowestPriority,
		LowerPriority,
		LowPriority,
		NormalPriority,
		HighPriority,
		HigherPriority,
		HighestPriority,
		TimeCriticalPriority,
		InheritPriority,
	};
	virtual ~Thread() {}

	static void setFactory(ThreadFactory* factory);

	static Thread* create(Runnable* r, size_t stackSize, Priority prio = InheritPriority, const char* name = "");
	static Thread* create(size_t stackSize, Priority prio = InheritPriority, const char* name = "");
	static void destroy(Thread* t);
	static void exit();
	static void sleep(unsigned long millis);
	static void yield();
	static Thread* getCurrentThread();

	virtual void start() = 0;
	virtual void start(Runnable* r) = 0;
	virtual void join() = 0;
	virtual bool isFinished() const = 0;

	virtual void setName(const char* name) = 0;
	virtual const char* getName() const = 0;
	virtual void setPriority(Priority prio) = 0;
	virtual Priority getPriority() const = 0;
	virtual size_t getStackSize() const = 0;

private:
	static ThreadFactory* m_factory;
};

}

#endif // OS_WRAPPER_THREAD_H_INCLUDED
