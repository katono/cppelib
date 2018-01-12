#ifndef WINDOWS_OS_WRAPPER_WINDOWS_THREAD_FACTORY_H_INCLUDED
#define WINDOWS_OS_WRAPPER_WINDOWS_THREAD_FACTORY_H_INCLUDED

#include "OSWrapper/ThreadFactory.h"
#include "private/WindowsThread.h"
#include <unordered_map>
#include <thread>
#include <mutex>

namespace WindowsOSWrapper {

class WindowsThreadFactory : public OSWrapper::ThreadFactory {
public:
	WindowsThreadFactory();
	virtual ~WindowsThreadFactory() {}

private:
	virtual OSWrapper::Thread* create(OSWrapper::Runnable* r, std::size_t stackSize, int priority, const char* name);
	virtual void destroy(OSWrapper::Thread* t);
	virtual void exit();
	virtual void sleep(unsigned long millis);
	virtual void yield();
	virtual OSWrapper::Thread* getCurrentThread();
	virtual int getPriorityMax() const;
	virtual int getPriorityMin() const;

	WindowsThread m_mainThread;
	std::unordered_map<std::thread::id, OSWrapper::Thread*> m_threadIdMap;
	std::mutex m_mutex;
};

}

#endif // WINDOWS_OS_WRAPPER_WINDOWS_THREAD_FACTORY_H_INCLUDED
