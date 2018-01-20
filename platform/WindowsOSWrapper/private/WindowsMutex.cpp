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

OSWrapper::Mutex::Error WindowsMutex::lock()
{
	try {
		m_mutex.lock();
		return OSWrapper::Mutex::OK;
	}
	catch (...) {
		return OSWrapper::Mutex::OtherError;
	}
}

OSWrapper::Mutex::Error WindowsMutex::tryLock()
{
	try {
		if (m_mutex.try_lock()) {
			return OSWrapper::Mutex::OK;
		}
		return OSWrapper::Mutex::TimedOut;
	}
	catch (...) {
		return OSWrapper::Mutex::OtherError;
	}
}

OSWrapper::Mutex::Error WindowsMutex::tryLockFor(OSWrapper::Timeout tmout)
{
	if (tmout == OSWrapper::Timeout::FOREVER) {
		return lock();
	}
	try {
		if (m_mutex.try_lock_for(std::chrono::milliseconds(tmout))) {
			return OSWrapper::Mutex::OK;
		}
		return OSWrapper::Mutex::TimedOut;
	}
	catch (...) {
		return OSWrapper::Mutex::OtherError;
	}
}

OSWrapper::Mutex::Error WindowsMutex::unlock()
{
	try {
		m_mutex.unlock();
		return OSWrapper::Mutex::OK;
	}
	catch (...) {
		return OSWrapper::Mutex::OtherError;
	}
}

}
