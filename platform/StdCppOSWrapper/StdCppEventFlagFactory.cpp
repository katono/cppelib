#include "StdCppEventFlagFactory.h"
#include "OSWrapper/EventFlag.h"
#include <condition_variable>
#include <chrono>

namespace StdCppOSWrapper {

using OSWrapper::EventFlag;
using OSWrapper::Timeout;

class StdCppEventFlag : public EventFlag {
private:
	bool m_autoReset;
	EventFlag::Pattern m_pattern;

	mutable std::mutex m_mutex;
	std::condition_variable m_cond;

public:
	StdCppEventFlag(bool autoReset)
	: m_autoReset(autoReset), m_pattern(), m_mutex(), m_cond()
	{
	}

	~StdCppEventFlag()
	{
	}

	OSWrapper::Error waitAny()
	{
		return timedWaitAny(Timeout::FOREVER);
	}

	OSWrapper::Error waitOne(std::size_t pos)
	{
		return timedWaitOne(pos, Timeout::FOREVER);
	}

	OSWrapper::Error wait(EventFlag::Pattern bitPattern, Mode waitMode, EventFlag::Pattern* releasedPattern)
	{
		return timedWait(bitPattern, waitMode, releasedPattern, Timeout::FOREVER);
	}

	OSWrapper::Error tryWaitAny()
	{
		return timedWaitAny(Timeout::POLLING);
	}

	OSWrapper::Error tryWaitOne(std::size_t pos)
	{
		return timedWaitOne(pos, Timeout::POLLING);
	}

	OSWrapper::Error tryWait(EventFlag::Pattern bitPattern, Mode waitMode, EventFlag::Pattern* releasedPattern)
	{
		return timedWait(bitPattern, waitMode, releasedPattern, Timeout::POLLING);
	}

	OSWrapper::Error timedWaitAny(Timeout tmout)
	{
		return timedWait(EventFlag::Pattern().set(), EventFlag::OR, nullptr, tmout);
	}

	OSWrapper::Error timedWaitOne(std::size_t pos, Timeout tmout)
	{
		if (pos >= EventFlag::Pattern().size()) {
			return OSWrapper::InvalidParameter;
		}
		return timedWait(EventFlag::Pattern().set(pos), EventFlag::OR, nullptr, tmout);
	}

	OSWrapper::Error timedWait(EventFlag::Pattern bitPattern, Mode waitMode, EventFlag::Pattern* releasedPattern, Timeout tmout)
	{
		if ((waitMode != EventFlag::OR) && (waitMode != EventFlag::AND)) {
			return OSWrapper::InvalidParameter;
		}
		if (bitPattern.none()) {
			return OSWrapper::InvalidParameter;
		}

		std::unique_lock<std::mutex> lock(m_mutex);

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
					return OSWrapper::TimedOut;
				}
			} else {
				if (!m_cond.wait_for(lock, std::chrono::milliseconds(tmout),
							[&] { return (bitPattern & m_pattern) == bitPattern; })) {
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
		return OSWrapper::OK;
	}

	OSWrapper::Error setAll()
	{
		return set(EventFlag::Pattern().set());
	}

	OSWrapper::Error setOne(std::size_t pos)
	{
		if (pos >= EventFlag::Pattern().size()) {
			return OSWrapper::InvalidParameter;
		}
		return set(EventFlag::Pattern().set(pos));
	}

	OSWrapper::Error set(EventFlag::Pattern bitPattern)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_pattern |= bitPattern;
		m_cond.notify_all();
		return OSWrapper::OK;
	}

	OSWrapper::Error resetAll()
	{
		return reset(EventFlag::Pattern().set());
	}

	OSWrapper::Error resetOne(std::size_t pos)
	{
		if (pos >= EventFlag::Pattern().size()) {
			return OSWrapper::InvalidParameter;
		}
		return reset(EventFlag::Pattern().set(pos));
	}

	OSWrapper::Error reset(EventFlag::Pattern bitPattern)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_pattern &= ~bitPattern;
		return OSWrapper::OK;
	}

	EventFlag::Pattern getCurrentPattern() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_pattern;
	}

};


StdCppEventFlagFactory::StdCppEventFlagFactory()
: m_mutex()
{
}

OSWrapper::EventFlag* StdCppEventFlagFactory::create(bool autoReset)
{
	try {
		std::lock_guard<std::mutex> lock(m_mutex);
		StdCppEventFlag* e = new StdCppEventFlag(autoReset);
		return e;
	}
	catch (...) {
		return nullptr;
	}
}

void StdCppEventFlagFactory::destroy(OSWrapper::EventFlag* e)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	delete static_cast<StdCppEventFlag*>(e);
}

}
