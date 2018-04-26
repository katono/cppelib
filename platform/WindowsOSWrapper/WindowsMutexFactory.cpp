#include "WindowsMutexFactory.h"
#include "OSWrapper/Mutex.h"
#include <chrono>

namespace WindowsOSWrapper {

class WindowsMutex : public OSWrapper::Mutex {
private:
	std::timed_mutex m_mutex;

public:
	WindowsMutex()
	: m_mutex()
	{
	}

	~WindowsMutex()
	{
	}

	OSWrapper::Error lock()
	{
		try {
			m_mutex.lock();
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
			m_mutex.unlock();
			return OSWrapper::OK;
		}
		catch (...) {
			return OSWrapper::OtherError;
		}
	}
};


WindowsMutexFactory::WindowsMutexFactory()
: m_mutex()
{
}

OSWrapper::Mutex* WindowsMutexFactory::create()
{
	try {
		std::lock_guard<std::mutex> lock(m_mutex);
		WindowsMutex* m = new WindowsMutex();
		return m;
	}
	catch (...) {
		return nullptr;
	}
}

OSWrapper::Mutex* WindowsMutexFactory::create(int priorityCeiling)
{
	(void) priorityCeiling;
	return create();
}

void WindowsMutexFactory::destroy(OSWrapper::Mutex* m)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	delete static_cast<WindowsMutex*>(m);
}

}
