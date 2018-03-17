#ifndef OS_WRAPPER_THREAD_H_INCLUDED
#define OS_WRAPPER_THREAD_H_INCLUDED

#include <cstddef>

namespace OSWrapper {

class Runnable;
class ThreadFactory;

void registerThreadFactory(ThreadFactory* factory);

class Thread {
public:
	static const int INHERIT_PRIORITY;

	virtual ~Thread() {}

	static Thread* create(Runnable* r, std::size_t stackSize = 0U, int priority = INHERIT_PRIORITY, const char* name = "");
	static Thread* create(std::size_t stackSize = 0U, int priority = INHERIT_PRIORITY, const char* name = "");
	static void destroy(Thread* t);
	static void exit();
	static void sleep(unsigned long millis);
	static void yield();
	static Thread* getCurrentThread();
	static int getPriorityMax();
	static int getPriorityMin();

	virtual void start() = 0;
	virtual void start(Runnable* r) = 0;
	virtual void join() = 0;
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
