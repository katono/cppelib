#include "WindowsThreadFactory.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/Runnable.h"
#include "Assertion/Assertion.h"
#include <thread>
#include <vector>
#include <condition_variable>
#include <chrono>
#include <windows.h>
#include <process.h>

namespace WindowsOSWrapper {

class WindowsThread : public OSWrapper::Thread {
private:
	int m_priority;
	int m_initialPriority;
	std::size_t m_stackSize;
	const char* m_name;

	std::thread m_thread;
	mutable std::mutex m_mutex;
	std::condition_variable m_condStarted;
	std::condition_variable m_condFinished;
	bool m_isActive;
	bool m_endThreadRequested;
	std::thread::id m_threadId;
	const std::unordered_map<int, int>& m_prioMap;

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

public:
	WindowsThread(OSWrapper::Runnable* r, int priority, std::size_t stackSize, const char* name, const std::unordered_map<int, int>& prioMap)
	: Thread(r), m_priority(priority), m_initialPriority(priority), m_stackSize(stackSize), m_name(name), 
	  m_thread(), m_mutex(), m_condStarted(), m_condFinished(), 
	  m_isActive(false), m_endThreadRequested(false), m_threadId(), m_prioMap(prioMap)
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
		m_initialPriority = m_priority;
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
		if ((OSWrapper::Thread::getMinPriority() <= priority) && (priority <= OSWrapper::Thread::getMaxPriority())) {
			std::lock_guard<std::mutex> lock(m_mutex);
			m_priority = priority;
			SetThreadPriority(m_thread.native_handle(), m_prioMap.at(priority));
			return;
		}

		// INHERIT_PRIORITY
		OSWrapper::Thread* t = OSWrapper::Thread::getCurrentThread();
		int this_priority;
		if (t == nullptr) {
			this_priority = OSWrapper::Thread::getNormalPriority();
		} else {
			this_priority = t->getPriority();
		}
		std::lock_guard<std::mutex> lock(m_mutex);
		m_priority = this_priority;
		SetThreadPriority(m_thread.native_handle(), m_prioMap.at(this_priority));
	}

	int getPriority() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_priority;
	}

	int getInitialPriority() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_initialPriority;
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


WindowsThreadFactory::WindowsThreadFactory(int lowestPriority, int highestPriority)
: m_threadIdMap(), m_mutex(), m_lowestPriority(0), m_highestPriority(0), m_prioMap()
{
	setPriorityRange(lowestPriority, highestPriority);
}

void WindowsThreadFactory::setPriorityRange(int lowestPriority, int highestPriority)
{
	m_lowestPriority = lowestPriority;
	m_highestPriority = highestPriority;

	m_prioMap.clear();
	m_prioMap.insert(std::make_pair(highestPriority, THREAD_PRIORITY_TIME_CRITICAL));
	m_prioMap.insert(std::make_pair(lowestPriority, THREAD_PRIORITY_IDLE));

	int mid = (lowestPriority + highestPriority) / 2;
	if (lowestPriority < highestPriority) {
		int diff = 0;
		for (int i = mid; i < highestPriority; ++i) {
			m_prioMap.insert(std::make_pair(i, THREAD_PRIORITY_NORMAL + diff));
			if ((THREAD_PRIORITY_NORMAL + diff) < THREAD_PRIORITY_HIGHEST) {
				diff++;
			}
		}

		diff = 1;
		for (int i = mid - 1; i > lowestPriority; --i) {
			m_prioMap.insert(std::make_pair(i, THREAD_PRIORITY_NORMAL - diff));
			if ((THREAD_PRIORITY_NORMAL - diff) > THREAD_PRIORITY_LOWEST) {
				diff++;
			}
		}
	} else {
		int diff = 0;
		for (int i = mid; i < lowestPriority; ++i) {
			m_prioMap.insert(std::make_pair(i, THREAD_PRIORITY_NORMAL - diff));
			if ((THREAD_PRIORITY_NORMAL - diff) > THREAD_PRIORITY_LOWEST) {
				diff++;
			}
		}

		diff = 1;
		for (int i = mid - 1; i > highestPriority; --i) {
			m_prioMap.insert(std::make_pair(i, THREAD_PRIORITY_NORMAL + diff));
			if ((THREAD_PRIORITY_NORMAL + diff) < THREAD_PRIORITY_HIGHEST) {
				diff++;
			}
		}
	}
}

OSWrapper::Thread* WindowsThreadFactory::create(OSWrapper::Runnable* r, int priority, std::size_t stackSize, void* stackAddress, const char* name)
{
	(void)stackAddress;
	WindowsThread* t = nullptr;
	try {
		t = new WindowsThread(r, priority, stackSize, name, m_prioMap);
		t->beginThread();
	}
	catch (const Assertion::Failure&) {
		throw;
	}
	catch (...) {
		delete t;
		return nullptr;
	}
	try {
		std::lock_guard<std::recursive_mutex> lock(m_mutex);
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
	WindowsThread* winThread = static_cast<WindowsThread*>(t);
	winThread->endThread();
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	m_threadIdMap.erase(winThread->getId());
	delete winThread;
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
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	auto iter = m_threadIdMap.find(std::this_thread::get_id());
	if (iter == m_threadIdMap.end()) {
		return nullptr;
	}
	return iter->second;
}

int WindowsThreadFactory::getMaxPriority() const
{
	if (m_lowestPriority < m_highestPriority) {
		return m_highestPriority;
	} else {
		return m_lowestPriority;
	}
}

int WindowsThreadFactory::getMinPriority() const
{
	if (m_lowestPriority < m_highestPriority) {
		return m_lowestPriority;
	} else {
		return m_highestPriority;
	}
}

int WindowsThreadFactory::getHighestPriority() const
{
	return m_highestPriority;
}

int WindowsThreadFactory::getLowestPriority() const
{
	return m_lowestPriority;
}

}
