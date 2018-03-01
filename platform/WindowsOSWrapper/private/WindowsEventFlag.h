#ifndef WINDOWS_OS_WRAPPER_WINDOWS_EVENT_FLAG_H_INCLUDED
#define WINDOWS_OS_WRAPPER_WINDOWS_EVENT_FLAG_H_INCLUDED

#include "OSWrapper/EventFlag.h"
#include <mutex>
#include <condition_variable>

namespace WindowsOSWrapper {

class WindowsEventFlag : public OSWrapper::EventFlag {
private:
	bool m_autoReset;
	OSWrapper::EventFlag::Pattern m_pattern;

	mutable std::mutex m_mutex;
	std::condition_variable m_cond;
	bool m_waiting;

public:
	WindowsEventFlag(bool autoReset);
	virtual ~WindowsEventFlag();

	virtual OSWrapper::Error waitAny(OSWrapper::Timeout tmout);
	virtual OSWrapper::Error waitOne(std::size_t pos, OSWrapper::Timeout tmout);
	virtual OSWrapper::Error wait(OSWrapper::EventFlag::Pattern bitPattern, Mode waitMode, OSWrapper::EventFlag::Pattern* releasedPattern, OSWrapper::Timeout tmout);

	virtual OSWrapper::Error setAll();
	virtual OSWrapper::Error setOne(std::size_t pos);
	virtual OSWrapper::Error set(OSWrapper::EventFlag::Pattern bitPattern);

	virtual OSWrapper::Error resetAll();
	virtual OSWrapper::Error resetOne(std::size_t pos);
	virtual OSWrapper::Error reset(OSWrapper::EventFlag::Pattern bitPattern);

	virtual OSWrapper::EventFlag::Pattern getCurrentPattern() const;

};

}

#endif // WINDOWS_OS_WRAPPER_WINDOWS_EVENT_FLAG_H_INCLUDED
