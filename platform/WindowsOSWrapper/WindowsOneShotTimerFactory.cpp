#include "WindowsOneShotTimerFactory.h"
#include "OSWrapper/OneShotTimer.h"
#include "OSWrapper/Runnable.h"
#include "OSWrapper/EventFlag.h"
#include <memory>
#include <windows.h>

namespace WindowsOSWrapper {

class WindowsOneShotTimer : public OSWrapper::OneShotTimer {
private:
	const char* m_name;

	mutable std::mutex m_mutex;
	bool m_started;
	MMRESULT m_timer;
	std::unique_ptr<OSWrapper::EventFlag, decltype(&OSWrapper::EventFlag::destroy)> m_evFinished;

	static void CALLBACK timerCallback(UINT, UINT, DWORD_PTR dwUser, DWORD_PTR, DWORD_PTR)
	{
		WindowsOneShotTimer* t = reinterpret_cast<WindowsOneShotTimer*>(dwUser);
		if (t != nullptr) {
			t->timerFunc();
		}
	}

	void timerFunc()
	{
		m_evFinished->resetAll();
		timerMain();
		std::lock_guard<std::mutex> lock(m_mutex);
		m_started = false;
		m_evFinished->setAll();
	}

public:
	WindowsOneShotTimer(OSWrapper::Runnable* r, const char* name)
	: OneShotTimer(r), m_name(name)
	, m_mutex(), m_started(false), m_timer(NULL)
	, m_evFinished(OSWrapper::EventFlag::create(false), &OSWrapper::EventFlag::destroy)
	{
		m_evFinished->setAll();
	}

	~WindowsOneShotTimer()
	{
		stop();
		m_evFinished->waitAny();
	}

	void start(unsigned long timeInMillis)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_started) {
			return;
		}
		MMRESULT timer = timeSetEvent(timeInMillis, 0, timerCallback, (DWORD) this, TIME_ONESHOT);
		if (timer == NULL) {
			return;
		}
		m_timer = timer;
		m_started = true;
	}

	void stop()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (!m_started) {
			return;
		}
		MMRESULT result = timeKillEvent(m_timer);
		if (result != TIMERR_NOERROR) {
			return;
		}
		m_started = false;
	}

	bool isStarted() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_started;
	}

	void setName(const char* name)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_name = name;
	}

	const char* getName() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_name;
	}

};


WindowsOneShotTimerFactory::WindowsOneShotTimerFactory()
: m_mutex()
{
}

OSWrapper::OneShotTimer* WindowsOneShotTimerFactory::create(OSWrapper::Runnable* r, const char* name)
{
	try {
		std::lock_guard<std::mutex> lock(m_mutex);
		WindowsOneShotTimer* t = new WindowsOneShotTimer(r, name);
		return t;
	}
	catch (...) {
		return nullptr;
	}
}

void WindowsOneShotTimerFactory::destroy(OSWrapper::OneShotTimer* t)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	delete static_cast<WindowsOneShotTimer*>(t);
}

}
