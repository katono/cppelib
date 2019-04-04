#include "WindowsThreadFactory.h"
#include <windows.h>
#include <process.h>

namespace WindowsOSWrapper {

WindowsThreadFactory::WindowsThread::WindowsThread(OSWrapper::Runnable* r, int priority, std::size_t stackSize, const char* name, const std::unordered_map<int, int>& prioMap)
: StdCppOSWrapper::StdCppThreadFactory::StdCppThread(r, priority, stackSize, name), m_prioMap(prioMap)
{
}

void WindowsThreadFactory::WindowsThread::setPriority(int priority)
{
	if ((OSWrapper::Thread::getMinPriority() <= priority) && (priority <= OSWrapper::Thread::getMaxPriority())) {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_priority = priority;
		SetThreadPriority(m_thread.native_handle(), m_prioMap.at(priority));
		return;
	}

	// INHERIT_PRIORITY
	OSWrapper::Thread* t = OSWrapper::Thread::getCurrentThread();
	int this_priority;
	if (t == nullptr) {
		this_priority = OSWrapper::Thread::getNormalPriority();
	} else {
		this_priority = t->getPriority();
	}
	std::lock_guard<std::mutex> lock(m_mutex);
	m_priority = this_priority;
	SetThreadPriority(m_thread.native_handle(), m_prioMap.at(this_priority));
}


WindowsThreadFactory::WindowsThreadFactory(int lowestPriority, int highestPriority)
: StdCppOSWrapper::StdCppThreadFactory(), m_lowestPriority(0), m_highestPriority(0), m_prioMap()
{
	setPriorityRange(lowestPriority, highestPriority);
}

void WindowsThreadFactory::setPriorityRange(int lowestPriority, int highestPriority)
{
	m_lowestPriority = lowestPriority;
	m_highestPriority = highestPriority;

	m_prioMap.clear();
	m_prioMap.insert(std::make_pair(highestPriority, THREAD_PRIORITY_TIME_CRITICAL));
	m_prioMap.insert(std::make_pair(lowestPriority, THREAD_PRIORITY_IDLE));

	int mid = (lowestPriority + highestPriority) / 2;
	if (lowestPriority < highestPriority) {
		int diff = 0;
		for (int i = mid; i < highestPriority; ++i) {
			m_prioMap.insert(std::make_pair(i, THREAD_PRIORITY_NORMAL + diff));
			if ((THREAD_PRIORITY_NORMAL + diff) < THREAD_PRIORITY_HIGHEST) {
				diff++;
			}
		}

		diff = 1;
		for (int i = mid - 1; i > lowestPriority; --i) {
			m_prioMap.insert(std::make_pair(i, THREAD_PRIORITY_NORMAL - diff));
			if ((THREAD_PRIORITY_NORMAL - diff) > THREAD_PRIORITY_LOWEST) {
				diff++;
			}
		}
	} else {
		int diff = 0;
		for (int i = mid; i < lowestPriority; ++i) {
			m_prioMap.insert(std::make_pair(i, THREAD_PRIORITY_NORMAL - diff));
			if ((THREAD_PRIORITY_NORMAL - diff) > THREAD_PRIORITY_LOWEST) {
				diff++;
			}
		}

		diff = 1;
		for (int i = mid - 1; i > highestPriority; --i) {
			m_prioMap.insert(std::make_pair(i, THREAD_PRIORITY_NORMAL + diff));
			if ((THREAD_PRIORITY_NORMAL + diff) < THREAD_PRIORITY_HIGHEST) {
				diff++;
			}
		}
	}
}

StdCppOSWrapper::StdCppThreadFactory::StdCppThread* WindowsThreadFactory::createStdCppThread(OSWrapper::Runnable* r, int priority, std::size_t stackSize, const char* name)
{
	return new WindowsThread(r, priority, stackSize, name, m_prioMap);
}

int WindowsThreadFactory::getMaxPriority() const
{
	if (m_lowestPriority < m_highestPriority) {
		return m_highestPriority;
	} else {
		return m_lowestPriority;
	}
}

int WindowsThreadFactory::getMinPriority() const
{
	if (m_lowestPriority < m_highestPriority) {
		return m_lowestPriority;
	} else {
		return m_highestPriority;
	}
}

int WindowsThreadFactory::getHighestPriority() const
{
	return m_highestPriority;
}

int WindowsThreadFactory::getLowestPriority() const
{
	return m_lowestPriority;
}

}
