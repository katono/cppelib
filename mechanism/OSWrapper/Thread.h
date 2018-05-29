#ifndef OS_WRAPPER_THREAD_H_INCLUDED
#define OS_WRAPPER_THREAD_H_INCLUDED

#include <cstddef>
#include <exception>
#include "Timeout.h"
#include "OSWrapperError.h"

namespace OSWrapper {

class Runnable;
class ThreadFactory;

void registerThreadFactory(ThreadFactory* factory);

class Thread {
protected:
	Thread(Runnable* r)
	: m_runnable(r), m_exceptionHandler(0) {}
	virtual ~Thread() {}

	void threadMain();

public:
	class ExceptionHandler {
	public:
		virtual ~ExceptionHandler() {}
		virtual void handle(Thread* t, const std::exception& e) = 0;
	};

	static const int INHERIT_PRIORITY;

	static void setDefaultExceptionHandler(ExceptionHandler* handler);
	static ExceptionHandler* getDefaultExceptionHandler();

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

	void setExceptionHandler(ExceptionHandler* handler);
	ExceptionHandler* getExceptionHandler() const;

private:
	Runnable* m_runnable;
	ExceptionHandler* m_exceptionHandler;
	static ExceptionHandler* m_defaultExceptionHandler;

	void handleException(const std::exception& e);

	class Exit {
	public:
		Exit() {}
	};

	class OtherException : public std::exception {
	public:
		explicit OtherException(const char* msg) : m_msg(msg) {}
		const char* what() const throw()
		{
			return m_msg;
		}
	private:
		const char* m_msg;
	};

};

}

#endif // OS_WRAPPER_THREAD_H_INCLUDED
