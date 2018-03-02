#include "WindowsMutex.h"
#include <chrono>

namespace WindowsOSWrapper {

WindowsMutex::WindowsMutex()
: m_mutex()
{
}

WindowsMutex::~WindowsMutex()
{
}

OSWrapper::Error WindowsMutex::lock()
{
	try {
		m_mutex.lock();
		return OSWrapper::OK;
	}
	catch (...) {
		return OSWrapper::OtherError;
	}
}

OSWrapper::Error WindowsMutex::tryLock()
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

OSWrapper::Error WindowsMutex::timedLock(OSWrapper::Timeout tmout)
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

OSWrapper::Error WindowsMutex::unlock()
{
	try {
		m_mutex.unlock();
		return OSWrapper::OK;
	}
	catch (...) {
		return OSWrapper::OtherError;
	}
}

}
