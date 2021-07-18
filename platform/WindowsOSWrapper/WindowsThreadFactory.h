#ifndef WINDOWS_OS_WRAPPER_WINDOWS_THREAD_FACTORY_H_INCLUDED
#define WINDOWS_OS_WRAPPER_WINDOWS_THREAD_FACTORY_H_INCLUDED

#include "StdCppOSWrapper/StdCppThreadFactory.h"
#include <string>

namespace WindowsOSWrapper {

class WindowsThreadFactory : public StdCppOSWrapper::StdCppThreadFactory {
private:
	class WindowsThread : public StdCppOSWrapper::StdCppThreadFactory::StdCppThread {
	private:
		const std::unordered_map<int, int>& m_prioMap;
		std::wstring m_threadName;
	public:
		WindowsThread(OSWrapper::Runnable* r, int priority, std::size_t stackSize, const char* name, const std::unordered_map<int, int>& prioMap);
		virtual ~WindowsThread() {}

		virtual void setName(const char* name);
		virtual void setPriority(int priority);
	};
public:
	WindowsThreadFactory(int lowestPriority = 1, int highestPriority = 9);
	virtual ~WindowsThreadFactory() {}

	void setPriorityRange(int lowestPriority, int highestPriority);

private:
	virtual int getMaxPriority() const;
	virtual int getMinPriority() const;
	virtual int getHighestPriority() const;
	virtual int getLowestPriority() const;

	virtual StdCppOSWrapper::StdCppThreadFactory::StdCppThread* createStdCppThread(OSWrapper::Runnable* r, int priority, std::size_t stackSize, const char* name);

	int m_lowestPriority;
	int m_highestPriority;
	std::unordered_map<int, int> m_prioMap;
};

}

#endif // WINDOWS_OS_WRAPPER_WINDOWS_THREAD_FACTORY_H_INCLUDED
