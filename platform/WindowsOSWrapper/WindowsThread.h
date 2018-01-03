#ifndef WINDOWS_OS_WRAPPER_WINDOWS_THREAD_H_INCLUDED
#define WINDOWS_OS_WRAPPER_WINDOWS_THREAD_H_INCLUDED

#include "OSWrapper/Thread.h"
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace WindowsOSWrapper {

class WindowsThread : public OSWrapper::Thread {
private:
	OSWrapper::Runnable* m_runnable;
	size_t m_stackSize;
	int m_priority;
	const char* m_name;

	std::thread m_thread;
	mutable std::mutex m_mutex;
	std::condition_variable m_condStarted;
	std::condition_variable m_condFinished;
	bool m_started;
	bool m_endThreadRequested;
	std::thread::id m_threadId;
	static const std::vector<int> m_prioList;

	static void threadEntry(WindowsThread* t);
	void threadLoop();
	void threadMain();

public:
	WindowsThread(OSWrapper::Runnable* r, size_t stackSize, int priority, const char* name);
	virtual ~WindowsThread();
	virtual void start();
	virtual void start(OSWrapper::Runnable* r);
	virtual void join();
	virtual bool isFinished() const;
	virtual void setName(const char* name);
	virtual const char* getName() const;
	virtual void setPriority(int priority);
	virtual int getPriority() const;
	virtual size_t getStackSize() const;
	void beginThread();
	void endThread();
	std::thread::id getId() const
	{
		return m_threadId;
	}

	static int getPriorityMax();
	static int getPriorityMin();

	class Exit {
	public:
		Exit() {}
	};
};

}

#endif // WINDOWS_OS_WRAPPER_WINDOWS_THREAD_H_INCLUDED
