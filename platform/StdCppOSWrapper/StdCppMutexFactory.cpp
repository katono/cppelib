#include "StdCppMutexFactory.h"
#include "OSWrapper/Mutex.h"
#include <chrono>
#include <cstddef>

namespace StdCppOSWrapper {

class StdCppMutex : public OSWrapper::Mutex {
private:
	std::recursive_timed_mutex m_mutex;
	std::size_t m_lockingCount;

public:
	StdCppMutex()
	: m_mutex(), m_lockingCount(0U)
	{
	}

	~StdCppMutex()
	{
	}

	OSWrapper::Error lock()
	{
		try {
			m_mutex.lock();
			m_lockingCount++;
			return OSWrapper::OK;
		}
		catch (...) {
			return OSWrapper::OtherError;
		}
	}

	OSWrapper::Error tryLock()
	{
		try {
			if (m_mutex.try_lock()) {
				m_lockingCount++;
				return OSWrapper::OK;
			}
			return OSWrapper::TimedOut;
		}
		catch (...) {
			return OSWrapper::OtherError;
		}
	}

	OSWrapper::Error timedLock(OSWrapper::Timeout tmout)
	{
		if (tmout == OSWrapper::Timeout::FOREVER) {
			return lock();
		}
		try {
			if (m_mutex.try_lock_for(std::chrono::milliseconds(tmout))) {
				m_lockingCount++;
				return OSWrapper::OK;
			}
			return OSWrapper::TimedOut;
		}
		catch (...) {
			return OSWrapper::OtherError;
		}
	}

	OSWrapper::Error unlock()
	{
		try {
			if (m_lockingCount == 0U) {
				return OSWrapper::NotLocked;
			}
			m_lockingCount--;
			m_mutex.unlock();
			return OSWrapper::OK;
		}
		catch (...) {
			return OSWrapper::OtherError;
		}
	}
};


StdCppMutexFactory::StdCppMutexFactory()
: m_mutex()
{
}

OSWrapper::Mutex* StdCppMutexFactory::create()
{
	try {
		std::lock_guard<std::mutex> lock(m_mutex);
		StdCppMutex* m = new StdCppMutex();
		return m;
	}
	catch (...) {
		return nullptr;
	}
}

OSWrapper::Mutex* StdCppMutexFactory::create(int priorityCeiling)
{
	(void) priorityCeiling;
	return create();
}

void StdCppMutexFactory::destroy(OSWrapper::Mutex* m)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	delete static_cast<StdCppMutex*>(m);
}

}
