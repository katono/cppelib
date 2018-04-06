#include "WindowsThreadFactory.h"
#include "Assertion/Assertion.h"
#include <chrono>

namespace WindowsOSWrapper {

WindowsThreadFactory::WindowsThreadFactory()
: m_mainThread(nullptr, 0, OSWrapper::Thread::INHERIT_PRIORITY, "MainThread"), 
  m_threadIdMap(), m_mutex()

{
	m_mainThread.setPriority(OSWrapper::Thread::INHERIT_PRIORITY);
	m_threadIdMap.insert(std::make_pair(std::this_thread::get_id(), &m_mainThread));
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
	if (t == &m_mainThread) {
		return;
	}
	std::lock_guard<std::mutex> lock(m_mutex);
	static_cast<WindowsThread*>(t)->endThread();
	m_threadIdMap.erase(static_cast<WindowsThread*>(t)->getId());
	delete t;
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
	DBC_ASSERT(iter != m_threadIdMap.end());
	return iter->second;
}

int WindowsThreadFactory::getPriorityMax() const
{
	return WindowsThread::getPriorityMax();
}

int WindowsThreadFactory::getPriorityMin() const
{
	return WindowsThread::getPriorityMin();
}

}
