#include "WindowsPeriodicTimerFactory.h"
#include "OSWrapper/PeriodicTimer.h"
#include "OSWrapper/Runnable.h"
#include "OSWrapper/EventFlag.h"
#include <memory>
#include <windows.h>

namespace WindowsOSWrapper {

class WindowsPeriodicTimer : public OSWrapper::PeriodicTimer {
private:
	unsigned long m_periodInMillis;
	const char* m_name;

	mutable std::mutex m_mutex;
	bool m_started;
	MMRESULT m_timer;
	std::unique_ptr<OSWrapper::EventFlag, decltype(&OSWrapper::EventFlag::destroy)> m_evFinished;

	static void CALLBACK timerCallback(UINT, UINT, DWORD_PTR dwUser, DWORD_PTR, DWORD_PTR)
	{
		WindowsPeriodicTimer* t = reinterpret_cast<WindowsPeriodicTimer*>(dwUser);
		if (t != nullptr) {
			t->m_evFinished->resetAll();
			t->timerMain();
			t->m_evFinished->setAll();
		}
	}

public:
	WindowsPeriodicTimer(OSWrapper::Runnable* r, unsigned long periodInMillis, const char* name)
	: PeriodicTimer(r), m_periodInMillis(periodInMillis), m_name(name)
	, m_mutex(), m_started(false), m_timer(NULL)
	, m_evFinished(OSWrapper::EventFlag::create(false), &OSWrapper::EventFlag::destroy)
	{
		m_evFinished->setAll();
	}

	~WindowsPeriodicTimer()
	{
		m_evFinished->waitAny();
	}

	void start()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_started) {
			return;
		}
		MMRESULT timer = timeSetEvent(m_periodInMillis, 0, timerCallback, (DWORD) this, TIME_PERIODIC);
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

	unsigned long getPeriodInMillis() const
	{
		return m_periodInMillis;
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


WindowsPeriodicTimerFactory::WindowsPeriodicTimerFactory()
: m_mutex()
{
}

OSWrapper::PeriodicTimer* WindowsPeriodicTimerFactory::create(OSWrapper::Runnable* r, unsigned long periodInMillis, const char* name)
{
	try {
		std::lock_guard<std::mutex> lock(m_mutex);
		WindowsPeriodicTimer* t = new WindowsPeriodicTimer(r, periodInMillis, name);
		return t;
	}
	catch (...) {
		return nullptr;
	}
}

void WindowsPeriodicTimerFactory::destroy(OSWrapper::PeriodicTimer* t)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	delete static_cast<WindowsPeriodicTimer*>(t);
}

}
