#include "PosixPeriodicTimerFactory.h"
#include "OSWrapper/PeriodicTimer.h"
#include "OSWrapper/Runnable.h"
#include "OSWrapper/EventFlag.h"
#include "OSWrapper/Thread.h"
#include <memory>
#include <time.h>
#include <signal.h>

namespace PosixOSWrapper {

class PosixPeriodicTimer : public OSWrapper::PeriodicTimer {
private:
	unsigned long m_periodInMillis;
	const char* m_name;

	mutable std::mutex m_mutex;
	bool m_started;
	timer_t m_timer;
	struct sigevent m_sigEvent;
	std::unique_ptr<OSWrapper::EventFlag, decltype(&OSWrapper::EventFlag::destroy)> m_flags;

	static const OSWrapper::EventFlag::Pattern EV_FINISHED;
	static const OSWrapper::EventFlag::Pattern EV_STOPPED;

	static void timerCallback(union sigval sv)
	{
		PosixPeriodicTimer* t = static_cast<PosixPeriodicTimer*>(sv.sival_ptr);
		if (t != nullptr) {
			t->timerFunc();
		}
	}

	void timerFunc()
	{
		m_flags->reset(EV_FINISHED);

		timerMain();

		if ((m_flags->getCurrentPattern() & EV_STOPPED) != 0) {
			const time_t sec = m_periodInMillis / 1000;
			const long nsec = (m_periodInMillis % 1000) * 1000000;
			struct itimerspec its;
			its.it_value.tv_sec = sec;
			its.it_value.tv_nsec = nsec;
			its.it_interval.tv_sec = 0;
			its.it_interval.tv_nsec = 0;
			timer_settime(m_timer, 0, &its, nullptr);
		}

		m_flags->set(EV_FINISHED);
	}

public:
	PosixPeriodicTimer(OSWrapper::Runnable* r, unsigned long periodInMillis, const char* name)
	: PeriodicTimer(r), m_periodInMillis(periodInMillis), m_name(name)
	, m_mutex(), m_started(false), m_timer(), m_sigEvent()
	, m_flags(OSWrapper::EventFlag::create(false), &OSWrapper::EventFlag::destroy)
	{
		m_flags->set(EV_FINISHED);

		m_sigEvent.sigev_value.sival_ptr = this;
		m_sigEvent.sigev_notify = SIGEV_THREAD;
		m_sigEvent.sigev_notify_function = timerCallback;
		m_sigEvent.sigev_notify_attributes = nullptr;
		timer_create(CLOCK_REALTIME, &m_sigEvent, &m_timer);
	}

	~PosixPeriodicTimer()
	{
		stop();
		m_flags->wait(EV_FINISHED, OSWrapper::EventFlag::OR, nullptr);
		timer_delete(m_timer);
		OSWrapper::Thread::sleep(m_periodInMillis);
		m_flags->wait(EV_FINISHED, OSWrapper::EventFlag::OR, nullptr);
	}

	void start()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_started) {
			return;
		}
		m_flags->reset(EV_STOPPED);
		const time_t sec = m_periodInMillis / 1000;
		const long nsec = (m_periodInMillis % 1000) * 1000000;
		struct itimerspec its;
		its.it_value.tv_sec = sec;
		its.it_value.tv_nsec = nsec;
		its.it_interval.tv_sec = 0;
		its.it_interval.tv_nsec = 0;
		timer_settime(m_timer, 0, &its, nullptr);
		m_started = true;
	}

	void stop()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (!m_started) {
			return;
		}
		m_flags->set(EV_STOPPED);
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

const OSWrapper::EventFlag::Pattern PosixPeriodicTimer::EV_FINISHED = 0x0001U;
const OSWrapper::EventFlag::Pattern PosixPeriodicTimer::EV_STOPPED = 0x0002U;


PosixPeriodicTimerFactory::PosixPeriodicTimerFactory()
: m_mutex()
{
}

OSWrapper::PeriodicTimer* PosixPeriodicTimerFactory::create(OSWrapper::Runnable* r, unsigned long periodInMillis, const char* name)
{
	try {
		std::lock_guard<std::mutex> lock(m_mutex);
		PosixPeriodicTimer* t = new PosixPeriodicTimer(r, periodInMillis, name);
		return t;
	}
	catch (...) {
		return nullptr;
	}
}

void PosixPeriodicTimerFactory::destroy(OSWrapper::PeriodicTimer* t)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	delete static_cast<PosixPeriodicTimer*>(t);
}

}
