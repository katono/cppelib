#ifndef POSIX_OS_WRAPPER_POSIX_THREAD_FACTORY_H_INCLUDED
#define POSIX_OS_WRAPPER_POSIX_THREAD_FACTORY_H_INCLUDED

#include "StdCppOSWrapper/StdCppThreadFactory.h"
#include <unistd.h>

namespace PosixOSWrapper {

class PosixThreadFactory : public StdCppOSWrapper::StdCppThreadFactory {
#ifdef _POSIX_PRIORITY_SCHEDULING
private:
	class PosixThread : public StdCppOSWrapper::StdCppThreadFactory::StdCppThread {
	private:
		int m_defaultPolicy;;
		int m_defaultPriority;
		const std::unordered_map<int, int>& m_prioMap;
	public:
		PosixThread(OSWrapper::Runnable* r, int priority, std::size_t stackSize, const char* name, int defaultPolicy, int defaultPriority, const std::unordered_map<int, int>& prioMap);
		virtual ~PosixThread() {}

		virtual void setName(const char* name);
		virtual void setPriority(int priority);
	};
public:
	PosixThreadFactory(int lowestPriority = 1, int highestPriority = 9);
	virtual ~PosixThreadFactory() {}

	void setPriorityRange(int lowestPriority, int highestPriority);

private:
	virtual int getMaxPriority() const;
	virtual int getMinPriority() const;
	virtual int getHighestPriority() const;
	virtual int getLowestPriority() const;

	virtual StdCppOSWrapper::StdCppThreadFactory::StdCppThread* createStdCppThread(OSWrapper::Runnable* r, int priority, std::size_t stackSize, const char* name);

	int m_defaultPolicy;;
	int m_defaultPriority;

	int m_lowestPriority;
	int m_highestPriority;
	std::unordered_map<int, int> m_prioMap;
#endif
};

}

#endif // POSIX_OS_WRAPPER_POSIX_THREAD_FACTORY_H_INCLUDED
