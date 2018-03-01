#include "WindowsEventFlag.h"
#include <chrono>

namespace WindowsOSWrapper {

using OSWrapper::EventFlag;
using OSWrapper::Timeout;

WindowsEventFlag::WindowsEventFlag(bool autoReset)
: m_autoReset(autoReset), m_pattern(), m_mutex(), m_cond(), m_waiting(false)
{
}

WindowsEventFlag::~WindowsEventFlag()
{
}

OSWrapper::Error WindowsEventFlag::waitAny(Timeout tmout)
{
	return wait(EventFlag::Pattern().set(), EventFlag::OR, nullptr, tmout);
}

OSWrapper::Error WindowsEventFlag::waitOne(std::size_t pos, Timeout tmout)
{
	if (pos >= EventFlag::Pattern().size()) {
		return OSWrapper::InvalidParameter;
	}
	return wait(EventFlag::Pattern().set(pos), EventFlag::OR, nullptr, tmout);
}

OSWrapper::Error WindowsEventFlag::wait(EventFlag::Pattern bitPattern, Mode waitMode, EventFlag::Pattern* releasedPattern, Timeout tmout)
{
	if ((waitMode != EventFlag::OR) && (waitMode != EventFlag::AND)) {
		return OSWrapper::InvalidParameter;
	}
	if (bitPattern.none()) {
		return OSWrapper::InvalidParameter;
	}

	std::unique_lock<std::mutex> lock(m_mutex);
	if (m_waiting) {
		return OSWrapper::OtherThreadWaiting;
	}
	m_waiting = true;

	if (tmout == Timeout::FOREVER) {
		if (waitMode == EventFlag::OR) {
			m_cond.wait(lock, [&] { return EventFlag::Pattern(bitPattern & m_pattern).any(); });
		} else {
			m_cond.wait(lock, [&] { return (bitPattern & m_pattern) == bitPattern; });
		}
	} else {
		if (waitMode == EventFlag::OR) {
			if (!m_cond.wait_for(lock, std::chrono::milliseconds(tmout),
						[&] { return EventFlag::Pattern(bitPattern & m_pattern).any(); })) {
				m_waiting = false;
				return OSWrapper::TimedOut;
			}
		} else {
			if (!m_cond.wait_for(lock, std::chrono::milliseconds(tmout),
						[&] { return (bitPattern & m_pattern) == bitPattern; })) {
				m_waiting = false;
				return OSWrapper::TimedOut;
			}
		}
	}

	if (releasedPattern != nullptr) {
		*releasedPattern = m_pattern;
	}
	if (m_autoReset) {
		m_pattern.reset();
	}
	m_waiting = false;
	return OSWrapper::OK;
}

OSWrapper::Error WindowsEventFlag::setAll()
{
	return set(EventFlag::Pattern().set());
}

OSWrapper::Error WindowsEventFlag::setOne(std::size_t pos)
{
	if (pos >= EventFlag::Pattern().size()) {
		return OSWrapper::InvalidParameter;
	}
	return set(EventFlag::Pattern().set(pos));
}

OSWrapper::Error WindowsEventFlag::set(EventFlag::Pattern bitPattern)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_pattern |= bitPattern;
	m_cond.notify_all();
	return OSWrapper::OK;
}

OSWrapper::Error WindowsEventFlag::resetAll()
{
	return reset(EventFlag::Pattern().set());
}

OSWrapper::Error WindowsEventFlag::resetOne(std::size_t pos)
{
	if (pos >= EventFlag::Pattern().size()) {
		return OSWrapper::InvalidParameter;
	}
	return reset(EventFlag::Pattern().set(pos));
}

OSWrapper::Error WindowsEventFlag::reset(EventFlag::Pattern bitPattern)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_pattern &= ~bitPattern;
	return OSWrapper::OK;
}

EventFlag::Pattern WindowsEventFlag::getCurrentPattern() const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_pattern;
}

}
