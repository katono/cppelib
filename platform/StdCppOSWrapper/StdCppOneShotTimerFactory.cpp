#include "StdCppOneShotTimerFactory.h"
#include "OSWrapper/OneShotTimer.h"
#include "OSWrapper/Runnable.h"
#include "OSWrapper/Thread.h"
#include <memory>
#include <chrono>
#include <condition_variable>

namespace StdCppOSWrapper {

class StdCppOneShotTimer : public OSWrapper::OneShotTimer {
private:
	unsigned long m_timeInMillis;
	const char* m_name;

	class TimerTask : public OSWrapper::Runnable {
	private:
		StdCppOneShotTimer* m_timer;
	public:
		TimerTask() : m_timer(0) {}
		virtual ~TimerTask() {}
		virtual void run()
		{
			m_timer->threadLoop();
		}
		void setTimer(StdCppOneShotTimer* timer)
		{
			m_timer = timer;
		}
	};
	TimerTask m_task;
	std::unique_ptr<OSWrapper::Thread, decltype(&OSWrapper::Thread::destroy)> m_thread;
	mutable std::mutex m_mutex;

	std::condition_variable m_condActive;
	std::condition_variable m_condStarted;
	std::condition_variable m_condStopped;
	bool m_isActive;
	bool m_started;
	bool m_stopped;
	bool m_endThreadRequested;

	void threadLoop()
	{
		while (true) {
			{
				std::unique_lock<std::mutex> lock(m_mutex);
				m_condActive.wait(lock, [this] { return m_isActive || m_endThreadRequested; });
				m_started = true;
				m_condStarted.notify_all();
				if (m_endThreadRequested) {
					break;
				}

				if (m_condActive.wait_for(lock, std::chrono::milliseconds(m_timeInMillis),
							[this] { return !m_isActive || m_endThreadRequested; })) {
					m_stopped = true;
					m_condStopped.notify_all();
					if (m_endThreadRequested) {
						break;
					}
					continue;
				}
			}
			timerMain();
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				m_started = false;
				m_stopped = true;
				m_isActive = false;
			}
		}
	}

public:
	StdCppOneShotTimer(OSWrapper::Runnable* r, const char* name)
	: OneShotTimer(r), m_timeInMillis(0U), m_name(name), 
	  m_task(), 
	  m_thread(OSWrapper::Thread::create(&m_task), &OSWrapper::Thread::destroy), 
	  m_mutex(), m_condActive(), m_condStarted(), m_condStopped(), 
	  m_isActive(false), m_started(false), m_stopped(true), m_endThreadRequested(false)
	{
	}

	~StdCppOneShotTimer()
	{
	}

	void beginThread()
	{
		m_task.setTimer(this);
		m_thread->setPriority(OSWrapper::Thread::getHighestPriority());
		m_thread->start();
	}

	void endThread()
	{
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_endThreadRequested = true;
			m_condActive.notify_all();
		}
		m_thread->wait();
	}

	void start(unsigned long timeInMillis)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		if (m_isActive) {
			return;
		}
		m_condStopped.wait(lock, [this] { return m_stopped; });
		m_timeInMillis = timeInMillis;
		m_stopped = false;
		m_isActive = true;
		m_condActive.notify_all();
	}

	void stop()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		if (!m_isActive) {
			return;
		}
		m_condStarted.wait(lock, [this] { return m_started; });
		m_started = false;
		m_isActive = false;
		m_condActive.notify_all();
	}

	bool isStarted() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_isActive;
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


StdCppOneShotTimerFactory::StdCppOneShotTimerFactory()
: m_mutex()
{
}

OSWrapper::OneShotTimer* StdCppOneShotTimerFactory::create(OSWrapper::Runnable* r, const char* name)
{
	try {
		std::lock_guard<std::mutex> lock(m_mutex);
		StdCppOneShotTimer* t = new StdCppOneShotTimer(r, name);
		t->beginThread();
		return t;
	}
	catch (...) {
		return nullptr;
	}
}

void StdCppOneShotTimerFactory::destroy(OSWrapper::OneShotTimer* t)
{
	StdCppOneShotTimer* timer = static_cast<StdCppOneShotTimer*>(t);
	timer->endThread();
	std::lock_guard<std::mutex> lock(m_mutex);
	delete static_cast<StdCppOneShotTimer*>(t);
}

}
