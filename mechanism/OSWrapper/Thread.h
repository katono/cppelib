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
protected:
	Thread(Runnable* r)
	: m_runnable(r), m_uncaughtExceptionHandler(0) {}
	virtual ~Thread() {}

	void threadMain();

public:
	class UncaughtExceptionHandler {
	public:
		virtual ~UncaughtExceptionHandler() {}
		virtual void handle(Thread* t, const char* msg) = 0;
	};

	static const int INHERIT_PRIORITY;

	static void setDefaultUncaughtExceptionHandler(UncaughtExceptionHandler* handler);
	static UncaughtExceptionHandler* getDefaultUncaughtExceptionHandler();

	static Thread* create(Runnable* r, int priority = INHERIT_PRIORITY, std::size_t stackSize = 0U, void* stackAddress = 0, const char* name = "");
	static void destroy(Thread* t);
	static void exit();
	static void sleep(unsigned long millis);
	static void yield();
	static Thread* getCurrentThread();
	static int getMaxPriority();
	static int getMinPriority();
	static int getNormalPriority();

	virtual void start() = 0;
	virtual Error wait() = 0;
	virtual Error tryWait() = 0;
	virtual Error timedWait(Timeout tmout) = 0;
	virtual bool isFinished() const = 0;

	virtual void setName(const char* name) = 0;
	virtual const char* getName() const = 0;
	virtual void setPriority(int priority) = 0;
	virtual int getPriority() const = 0;
	virtual int getInitialPriority() const = 0;
	virtual std::size_t getStackSize() const = 0;
	virtual void* getNativeHandle() = 0;

	void setUncaughtExceptionHandler(UncaughtExceptionHandler* handler);
	UncaughtExceptionHandler* getUncaughtExceptionHandler() const;

private:
	Runnable* m_runnable;
	UncaughtExceptionHandler* m_uncaughtExceptionHandler;
	static UncaughtExceptionHandler* m_defaultUncaughtExceptionHandler;

	void handleException(const char* msg);

	class Exit {
	public:
		Exit() {}
	};

};

}

#endif // OS_WRAPPER_THREAD_H_INCLUDED
