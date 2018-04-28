#include "WindowsThreadFactory.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/Runnable.h"
#include "Assertion/Assertion.h"
#include <vector>
#include <condition_variable>
#include <chrono>
#include <exception>
#include <windows.h>
#include <process.h>
#include <iostream>

namespace WindowsOSWrapper {

const std::vector<int> s_prioList = {
	THREAD_PRIORITY_IDLE,
	THREAD_PRIORITY_LOWEST - 1,
	THREAD_PRIORITY_LOWEST,
	THREAD_PRIORITY_BELOW_NORMAL,
	THREAD_PRIORITY_NORMAL,
	THREAD_PRIORITY_ABOVE_NORMAL,
	THREAD_PRIORITY_HIGHEST,
	THREAD_PRIORITY_HIGHEST + 1,
	THREAD_PRIORITY_TIME_CRITICAL,
};

class WindowsThread : public OSWrapper::Thread {
private:
	OSWrapper::Runnable* m_runnable;
	std::size_t m_stackSize;
	int m_priority;
	const char* m_name;

	std::thread m_thread;
	mutable std::mutex m_mutex;
	std::condition_variable m_condStarted;
	std::condition_variable m_condFinished;
	bool m_isActive;
	bool m_endThreadRequested;
	std::thread::id m_threadId;

	static void threadEntry(WindowsThread* t)
	{
		if (t != nullptr) {
			t->threadLoop();
		}
	}

	void threadLoop()
	{
		while (true) {
			{
				std::unique_lock<std::mutex> lock(m_mutex);
				m_condStarted.wait(lock, [this] { return m_isActive; });
				if (m_endThreadRequested) {
					break;
				}
			}
			threadMain();
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				m_isActive = false;
				m_condFinished.notify_all();
			}
		}
	}

	void threadMain()
	{
		try {
			if (m_runnable != nullptr) {
				m_runnable->run();
			}
		}
		catch (const Exit&) {
			// do nothing
		}
		catch (const std::exception& e) {
			std::cout << e.what() << std::endl;
		}
		catch (...) {
			std::cout << "Unknown Exception" << std::endl;
		}
	}

public:
	class Exit {
	public:
		Exit() {}
	};

	WindowsThread(OSWrapper::Runnable* r, std::size_t stackSize, int priority, const char* name)
	: m_runnable(r), m_stackSize(stackSize), m_priority(priority), m_name(name), 
	  m_thread(), m_mutex(), m_condStarted(), m_condFinished(), 
	  m_isActive(false), m_endThreadRequested(false), m_threadId()
	{
	}

	~WindowsThread()
	{
	}

	void beginThread()
	{
		std::thread t(&threadEntry, this);
		t.swap(m_thread);
		m_threadId = m_thread.get_id();
		setPriority(m_priority);
	}

	void endThread()
	{
		wait();
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_endThreadRequested = true;
			m_isActive = true;
			m_condStarted.notify_all();
		}
		m_thread.join();
	}

	void start()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_isActive) {
			return;
		}
		m_isActive = true;
		m_condStarted.notify_all();
	}

	OSWrapper::Error wait()
	{
		return timedWait(OSWrapper::Timeout::FOREVER);
	}

	OSWrapper::Error tryWait()
	{
		return timedWait(OSWrapper::Timeout::POLLING);
	}

	OSWrapper::Error timedWait(OSWrapper::Timeout tmout)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		if (tmout == OSWrapper::Timeout::FOREVER) {
			m_condFinished.wait(lock, [this] { return !m_isActive; });
		} else {
			if (!m_condFinished.wait_for(lock, std::chrono::milliseconds(tmout),
						[this] { return !m_isActive; })) {
				return OSWrapper::TimedOut;
			}
		}
		return OSWrapper::OK;
	}

	bool isFinished() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return !m_isActive;
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

	void setPriority(int priority)
	{
		if ((0 <= priority) && (priority < static_cast<int>(s_prioList.size()))) {
			std::lock_guard<std::mutex> lock(m_mutex);
			m_priority = priority;
			SetThreadPriority(m_thread.native_handle(), s_prioList[priority]);
			return;
		}

		// INHERIT_PRIORITY
		const auto winPriority = GetThreadPriority(GetCurrentThread());
		for (int i = 0; i < static_cast<int>(s_prioList.size()); i++) {
			if (s_prioList[i] == winPriority) {
				std::lock_guard<std::mutex> lock(m_mutex);
				m_priority = i;
				SetThreadPriority(m_thread.native_handle(), winPriority);
				break;
			}
		}
	}

	int getPriority() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_priority;
	}

	std::size_t getStackSize() const
	{
		if (m_stackSize == 0U) {
			return 1024U * 1024U;
		}
		return m_stackSize;
	}

	void* getNativeHandle()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_thread.native_handle();
	}

	std::thread::id getId() const
	{
		return m_threadId;
	}
};


WindowsThreadFactory::WindowsThreadFactory()
: m_threadIdMap(), m_mutex()
{
}

OSWrapper::Thread* WindowsThreadFactory::create(OSWrapper::Runnable* r, std::size_t stackSize, int priority, const char* name)
{
	WindowsThread* t = nullptr;
	std::lock_guard<std::mutex> lock(m_mutex);
	try {
		t = new WindowsThread(r, stackSize, priority, name);
		t->beginThread();
	}
	catch (...) {
		delete t;
		return nullptr;
	}
	try {
		m_threadIdMap.insert(std::make_pair(t->getId(), t));
		return t;
	}
	catch (...) {
		t->endThread();
		delete t;
		return nullptr;
	}
}

void WindowsThreadFactory::destroy(OSWrapper::Thread* t)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	WindowsThread* winThread = static_cast<WindowsThread*>(t);
	winThread->endThread();
	m_threadIdMap.erase(winThread->getId());
	delete winThread;
}

void WindowsThreadFactory::exit()
{
	throw WindowsThread::Exit();
}

void WindowsThreadFactory::sleep(unsigned long millis)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(millis));
}

void WindowsThreadFactory::yield()
{
	std::this_thread::yield();
}

OSWrapper::Thread* WindowsThreadFactory::getCurrentThread()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	auto iter = m_threadIdMap.find(std::this_thread::get_id());
	CHECK_ASSERT(iter != m_threadIdMap.end());
	return iter->second;
}

int WindowsThreadFactory::getMaxPriority() const
{
	return static_cast<int>(s_prioList.size()) - 1;
}

int WindowsThreadFactory::getMinPriority() const
{
	return 0;
}

}
