#ifndef WINDOWS_OS_WRAPPER_WINDOWS_THREAD_FACTORY_H_INCLUDED
#define WINDOWS_OS_WRAPPER_WINDOWS_THREAD_FACTORY_H_INCLUDED

#include "OSWrapper/ThreadFactory.h"
#include <unordered_map>
#include <mutex>

namespace WindowsOSWrapper {

class WindowsThreadFactory : public OSWrapper::ThreadFactory {
public:
	WindowsThreadFactory(int lowestPriority = 1, int highestPriority = 9);
	virtual ~WindowsThreadFactory() {}

	void setPriorityRange(int lowestPriority, int highestPriority);

private:
	virtual OSWrapper::Thread* create(OSWrapper::Runnable* r, std::size_t stackSize, int priority, const char* name);
	virtual void destroy(OSWrapper::Thread* t);
	virtual void sleep(unsigned long millis);
	virtual void yield();
	virtual OSWrapper::Thread* getCurrentThread();
	virtual int getMaxPriority() const;
	virtual int getMinPriority() const;

	WindowsThreadFactory(const WindowsThreadFactory&);
	WindowsThreadFactory& operator=(const WindowsThreadFactory&);

	std::unordered_map<std::thread::id, OSWrapper::Thread*> m_threadIdMap;
	std::recursive_mutex m_mutex;
	int m_lowestPriority;
	int m_highestPriority;
	std::unordered_map<int, int> m_prioMap;
};

}

#endif // WINDOWS_OS_WRAPPER_WINDOWS_THREAD_FACTORY_H_INCLUDED
