#include "WindowsThread.h"
#include "OSWrapper/Runnable.h"
#include <exception>
#include <windows.h>
#include <process.h>
#include <iostream>

namespace WindowsOSWrapper {

const std::vector<int> WindowsThread::m_prioList = {
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

WindowsThread::WindowsThread(OSWrapper::Runnable* r, std::size_t stackSize, int priority, const char* name)
: m_runnable(r), m_stackSize(stackSize), m_priority(priority), m_name(name), 
  m_thread(), m_mutex(), m_condStarted(), m_condFinished(), 
  m_started(false), m_endThreadRequested(false), m_threadId()
{
}

WindowsThread::~WindowsThread()
{
}

void WindowsThread::threadEntry(WindowsThread* t)
{
	if (t != nullptr) {
		t->threadLoop();
	}
}

void WindowsThread::threadLoop()
{
	while (true) {
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			while (!m_started) {
				m_condStarted.wait(lock);
			}
			if (m_endThreadRequested) {
				break;
			}
		}
		threadMain();
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_started = false;
			m_condFinished.notify_all();
		}
	}
}

void WindowsThread::threadMain()
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

void WindowsThread::beginThread()
{
	std::thread t(&threadEntry, this);
	t.swap(m_thread);
	m_threadId = m_thread.get_id();
	setPriority(m_priority);
}

void WindowsThread::endThread()
{
	join();
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_endThreadRequested = true;
		m_started = true;
		m_condStarted.notify_all();
	}
	m_thread.join();
}

void WindowsThread::start()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	if (m_started) {
		return;
	}
	m_started = true;
	m_condStarted.notify_all();
}

void WindowsThread::start(OSWrapper::Runnable* r)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	if (m_started) {
		return;
	}
	m_runnable = r;
	m_started = true;
	m_condStarted.notify_all();
}

void WindowsThread::join()
{
	std::unique_lock<std::mutex> lock(m_mutex);
	while (m_started) {
		m_condFinished.wait(lock);
	}
}

bool WindowsThread::isFinished() const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return !m_started;
}

void WindowsThread::setName(const char* name)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_name = name;
}

const char* WindowsThread::getName() const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_name;
}

void WindowsThread::setPriority(int priority)
{
	if ((0 <= priority) && (priority < static_cast<int>(m_prioList.size()))) {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_priority = priority;
		SetThreadPriority(m_thread.native_handle(), m_prioList[priority]);
		return;
	}

	// INHERIT_PRIORITY
	const auto winPriority = GetThreadPriority(GetCurrentThread());
	for (int i = 0; i < static_cast<int>(m_prioList.size()); i++) {
		if (m_prioList[i] == winPriority) {
			std::lock_guard<std::mutex> lock(m_mutex);
			m_priority = i;
			SetThreadPriority(m_thread.native_handle(), winPriority);
			break;
		}
	}
}

int WindowsThread::getPriority() const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_priority;
}

std::size_t WindowsThread::getStackSize() const
{
	if (m_stackSize == 0U) {
		return 1024U * 1024U;
	}
	return m_stackSize;
}

void* WindowsThread::getNativeHandle()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_thread.native_handle();
}

int WindowsThread::getPriorityMax()
{
	return static_cast<int>(m_prioList.size()) - 1;
}

int WindowsThread::getPriorityMin()
{
	return 0;
}

}
