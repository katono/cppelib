#include "PosixThreadFactory.h"

#ifdef _POSIX_PRIORITY_SCHEDULING
#include <pthread.h>
#include <sched.h>

namespace PosixOSWrapper {

PosixThreadFactory::PosixThread::PosixThread(OSWrapper::Runnable* r, int priority, std::size_t stackSize, const char* name,
		int defaultPolicy, int defaultPriority, const std::unordered_map<int, int>& prioMap)
: StdCppOSWrapper::StdCppThreadFactory::StdCppThread(r, priority, stackSize, name),
  m_defaultPolicy(defaultPolicy), m_defaultPriority(defaultPriority), m_prioMap(prioMap)
{
}

void PosixThreadFactory::PosixThread::setName(const char* name)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_name = name;
#ifdef __GNUC__
	pthread_setname_np(m_thread.native_handle(), name);
#endif
}

void PosixThreadFactory::PosixThread::setPriority(int priority)
{
	if ((OSWrapper::Thread::getMinPriority() <= priority) && (priority <= OSWrapper::Thread::getMaxPriority())) {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_priority = priority;
		struct sched_param param;
		param.sched_priority = m_prioMap.at(priority);
		int policy = (param.sched_priority == m_defaultPriority) ? m_defaultPolicy : SCHED_RR;
		pthread_setschedparam(m_thread.native_handle(), policy, &param);
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
	struct sched_param param;
	param.sched_priority = m_prioMap.at(this_priority);
	int policy = (param.sched_priority == m_defaultPriority) ? m_defaultPolicy : SCHED_RR;
	pthread_setschedparam(m_thread.native_handle(), policy, &param);
}


PosixThreadFactory::PosixThreadFactory(int lowestPriority, int highestPriority)
: StdCppOSWrapper::StdCppThreadFactory(), m_defaultPolicy(SCHED_OTHER), m_defaultPriority(0),
  m_lowestPriority(0), m_highestPriority(0), m_prioMap()
{
	int policy;
	struct sched_param param;
	int ret = pthread_getschedparam(pthread_self(), &policy, &param);
	if (ret != 0) {
		return;
	}
	m_defaultPolicy = policy;
	m_defaultPriority = param.sched_priority;

	setPriorityRange(lowestPriority, highestPriority);
}

void PosixThreadFactory::setPriorityRange(int lowestPriority, int highestPriority)
{
	m_lowestPriority = lowestPriority;
	m_highestPriority = highestPriority;

	m_prioMap.clear();

	const int posixHighestPriority = sched_get_priority_max(SCHED_RR);
	const int posixAboveMidPriority = sched_get_priority_min(SCHED_RR);

	const int mid = (lowestPriority + highestPriority) / 2;

	if (lowestPriority < highestPriority) {
		int posixPriority = posixAboveMidPriority;
		for (int i = mid + 1; i <= highestPriority; ++i) {
			m_prioMap.insert(std::make_pair(i, posixPriority));
			if (posixPriority < posixHighestPriority) {
				posixPriority++;
			}
		}

		for (int i = mid; i >= lowestPriority; --i) {
			m_prioMap.insert(std::make_pair(i, m_defaultPriority));
		}
	} else {
		for (int i = mid; i <= lowestPriority; ++i) {
			m_prioMap.insert(std::make_pair(i, m_defaultPriority));
		}

		int posixPriority = posixAboveMidPriority;
		for (int i = mid - 1; i >= highestPriority; --i) {
			m_prioMap.insert(std::make_pair(i, posixPriority));
			if (posixPriority < posixHighestPriority) {
				posixPriority++;
			}
		}
	}
}

StdCppOSWrapper::StdCppThreadFactory::StdCppThread* PosixThreadFactory::createStdCppThread(OSWrapper::Runnable* r, int priority, std::size_t stackSize, const char* name)
{
	return new PosixThread(r, priority, stackSize, name, m_defaultPolicy, m_defaultPriority, m_prioMap);
}

int PosixThreadFactory::getMaxPriority() const
{
	if (m_lowestPriority < m_highestPriority) {
		return m_highestPriority;
	} else {
		return m_lowestPriority;
	}
}

int PosixThreadFactory::getMinPriority() const
{
	if (m_lowestPriority < m_highestPriority) {
		return m_lowestPriority;
	} else {
		return m_highestPriority;
	}
}

int PosixThreadFactory::getHighestPriority() const
{
	return m_highestPriority;
}

int PosixThreadFactory::getLowestPriority() const
{
	return m_lowestPriority;
}

}

#endif
