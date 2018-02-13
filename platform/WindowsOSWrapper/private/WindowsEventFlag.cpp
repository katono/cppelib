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

EventFlag::Error WindowsEventFlag::waitAny(Timeout tmout)
{
	return wait(EventFlag::Pattern().set(), EventFlag::OR, nullptr, tmout);
}

EventFlag::Error WindowsEventFlag::waitOne(std::size_t pos, Timeout tmout)
{
	if (pos >= EventFlag::Pattern().size()) {
		return EventFlag::InvalidParameter;
	}
	return wait(EventFlag::Pattern().set(pos), EventFlag::OR, nullptr, tmout);
}

EventFlag::Error WindowsEventFlag::wait(EventFlag::Pattern bitPattern, Mode waitMode, EventFlag::Pattern* releasedPattern, Timeout tmout)
{
	if ((waitMode != EventFlag::OR) && (waitMode != EventFlag::AND)) {
		return EventFlag::InvalidParameter;
	}
	if (bitPattern.none()) {
		return EventFlag::InvalidParameter;
	}

	std::unique_lock<std::mutex> lock(m_mutex, std::try_to_lock);
	if (!lock) {
		return EventFlag::OtherThreadWaiting;
	}
	if (m_waiting) {
		return EventFlag::OtherThreadWaiting;
	}
	m_waiting = true;

	if (tmout == Timeout::FOREVER) {
		if (waitMode == EventFlag::OR) {
			m_cond.wait(lock, [&] { return (bitPattern & m_pattern).any(); });
		} else {
			m_cond.wait(lock, [&] { return (bitPattern & m_pattern) == bitPattern; });
		}
	} else {
		if (waitMode == EventFlag::OR) {
			if (!m_cond.wait_for(lock, std::chrono::milliseconds(tmout),
						[&] { return (bitPattern & m_pattern).any(); })) {
				m_waiting = false;
				return EventFlag::TimedOut;
			}
		} else {
			if (!m_cond.wait_for(lock, std::chrono::milliseconds(tmout),
						[&] { return (bitPattern & m_pattern) == bitPattern; })) {
				m_waiting = false;
				return EventFlag::TimedOut;
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
	return EventFlag::OK;
}

EventFlag::Error WindowsEventFlag::setAll()
{
	return set(EventFlag::Pattern().set());
}

EventFlag::Error WindowsEventFlag::setOne(std::size_t pos)
{
	if (pos >= EventFlag::Pattern().size()) {
		return EventFlag::InvalidParameter;
	}
	return set(EventFlag::Pattern().set(pos));
}

EventFlag::Error WindowsEventFlag::set(EventFlag::Pattern bitPattern)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_pattern |= bitPattern;
	m_cond.notify_all();
	return EventFlag::OK;
}

EventFlag::Error WindowsEventFlag::resetAll()
{
	return reset(EventFlag::Pattern().set());
}

EventFlag::Error WindowsEventFlag::resetOne(std::size_t pos)
{
	if (pos >= EventFlag::Pattern().size()) {
		return EventFlag::InvalidParameter;
	}
	return reset(EventFlag::Pattern().set(pos));
}

EventFlag::Error WindowsEventFlag::reset(EventFlag::Pattern bitPattern)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_pattern &= ~bitPattern;
	return EventFlag::OK;
}

EventFlag::Pattern WindowsEventFlag::getCurrentPattern() const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_pattern;
}

}
