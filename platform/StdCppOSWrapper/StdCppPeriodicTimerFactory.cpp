#include "StdCppPeriodicTimerFactory.h"
#include "OSWrapper/PeriodicTimer.h"
#include "OSWrapper/Runnable.h"
#include "OSWrapper/Thread.h"
#include <memory>
#include <chrono>
#include <condition_variable>

namespace StdCppOSWrapper {

class StdCppPeriodicTimer : public OSWrapper::PeriodicTimer {
private:
	unsigned long m_periodInMillis;
	const char* m_name;

	class TimerTask : public OSWrapper::Runnable {
	private:
		StdCppPeriodicTimer* m_timer;
	public:
		explicit TimerTask(StdCppPeriodicTimer* timer) : m_timer(timer) {}
		virtual ~TimerTask() {}
		virtual void run()
		{
			m_timer->threadLoop();
		}
	};
	TimerTask m_task;
	std::unique_ptr<OSWrapper::Thread, decltype(&OSWrapper::Thread::destroy)> m_thread;
	mutable std::mutex m_mutex;

	std::condition_variable m_condStarted;
	std::condition_variable m_condStopped;
	bool m_isActive;
	bool m_stopped;
	bool m_endThreadRequested;

	void threadLoop()
	{
		while (true) {
			{
				std::unique_lock<std::mutex> lock(m_mutex);
				m_condStarted.wait(lock, [this] { return m_isActive || m_endThreadRequested; });
				if (m_endThreadRequested) {
					break;
				}
			}
			while (true) {
				{
					std::unique_lock<std::mutex> lock(m_mutex);
					if (m_condStarted.wait_for(lock, std::chrono::milliseconds(m_periodInMillis),
								[this] { return !m_isActive || m_endThreadRequested; })) {
						m_stopped = true;
						m_condStopped.notify_all();
						if (m_endThreadRequested) {
							return;
						}
						break;
					}
				}
				timerMain();
			}
		}
	}

public:
	StdCppPeriodicTimer(OSWrapper::Runnable* r, unsigned long periodInMillis, const char* name)
	: PeriodicTimer(r), m_periodInMillis(periodInMillis), m_name(name), 
	  m_task(this), 
	  m_thread(OSWrapper::Thread::create(&m_task), &OSWrapper::Thread::destroy), 
	  m_mutex(), m_condStarted(), m_condStopped(), 
	  m_isActive(false), m_stopped(true), m_endThreadRequested(false)
	{
		m_thread->setPriority(OSWrapper::Thread::getHighestPriority());
		m_thread->start();
	}

	~StdCppPeriodicTimer()
	{
	}

	void endThread()
	{
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_endThreadRequested = true;
			m_condStarted.notify_all();
		}
		m_thread->wait();
	}

	void start()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		if (m_isActive) {
			return;
		}
		m_condStopped.wait(lock, [this] { return m_stopped; });
		m_stopped = false;
		m_isActive = true;
		m_condStarted.notify_all();
	}

	void stop()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (!m_isActive) {
			return;
		}
		m_isActive = false;
		m_condStarted.notify_all();
	}

	bool isStarted() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_isActive;
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


StdCppPeriodicTimerFactory::StdCppPeriodicTimerFactory()
: m_mutex()
{
}

OSWrapper::PeriodicTimer* StdCppPeriodicTimerFactory::create(OSWrapper::Runnable* r, unsigned long periodInMillis, const char* name)
{
	try {
		std::lock_guard<std::mutex> lock(m_mutex);
		return new StdCppPeriodicTimer(r, periodInMillis, name);
	}
	catch (...) {
		return nullptr;
	}
}

void StdCppPeriodicTimerFactory::destroy(OSWrapper::PeriodicTimer* t)
{
	StdCppPeriodicTimer* timer = static_cast<StdCppPeriodicTimer*>(t);
	timer->endThread();
	std::lock_guard<std::mutex> lock(m_mutex);
	delete static_cast<StdCppPeriodicTimer*>(t);
}

}
