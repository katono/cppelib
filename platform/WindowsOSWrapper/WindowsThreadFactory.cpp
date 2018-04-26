#include "WindowsThreadFactory.h"
#include "private/WindowsThread.h"
#include "Assertion/Assertion.h"
#include <chrono>

namespace WindowsOSWrapper {

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

int WindowsThreadFactory::getHighestPriority() const
{
	return WindowsThread::getHighestPriority();
}

int WindowsThreadFactory::getLowestPriority() const
{
	return WindowsThread::getLowestPriority();
}

}
