#include "StdCppThreadFactory.h"
#include "OSWrapper/Runnable.h"
#include "Assertion/Assertion.h"
#include <chrono>

namespace StdCppOSWrapper {

void StdCppThreadFactory::StdCppThread::threadEntry(StdCppThread* t)
{
	if (t != nullptr) {
		t->threadLoop();
	}
}

void StdCppThreadFactory::StdCppThread::threadLoop()
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

StdCppThreadFactory::StdCppThread::StdCppThread(OSWrapper::Runnable* r, int priority, std::size_t stackSize, const char* name)
: Thread(r), m_priority(priority), m_initialPriority(priority), m_stackSize(stackSize), m_name(name), 
  m_thread(), m_mutex(), m_condStarted(), m_condFinished(), 
  m_isActive(false), m_endThreadRequested(false), m_threadId()
{
}

void StdCppThreadFactory::StdCppThread::beginThread()
{
	std::thread t(&threadEntry, this);
	t.swap(m_thread);
	m_threadId = m_thread.get_id();
	setPriority(m_priority);
	m_initialPriority = m_priority;
}

void StdCppThreadFactory::StdCppThread::endThread()
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

void StdCppThreadFactory::StdCppThread::start()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	if (m_isActive) {
		return;
	}
	m_isActive = true;
	m_condStarted.notify_all();
}

OSWrapper::Error StdCppThreadFactory::StdCppThread::wait()
{
	return timedWait(OSWrapper::Timeout::FOREVER);
}

OSWrapper::Error StdCppThreadFactory::StdCppThread::tryWait()
{
	return timedWait(OSWrapper::Timeout::POLLING);
}

OSWrapper::Error StdCppThreadFactory::StdCppThread::timedWait(OSWrapper::Timeout tmout)
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

bool StdCppThreadFactory::StdCppThread::isFinished() const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return !m_isActive;
}

void StdCppThreadFactory::StdCppThread::setName(const char* name)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_name = name;
}

const char* StdCppThreadFactory::StdCppThread::getName() const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_name;
}

void StdCppThreadFactory::StdCppThread::setPriority(int priority)
{
	// Can not set real priority by this class
	if (priority != OSWrapper::Thread::INHERIT_PRIORITY) {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_priority = priority;
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
}

int StdCppThreadFactory::StdCppThread::getPriority() const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_priority;
}

int StdCppThreadFactory::StdCppThread::getInitialPriority() const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_initialPriority;
}

std::size_t StdCppThreadFactory::StdCppThread::getStackSize() const
{
	// Can not get real stack size by this class
	if (m_stackSize == 0U) {
		return 1024U * 1024U;
	}
	return m_stackSize;
}

void* StdCppThreadFactory::StdCppThread::getNativeHandle()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return reinterpret_cast<void*>(m_thread.native_handle());
}


StdCppThreadFactory::StdCppThreadFactory()
: m_threadIdMap(), m_mutex()
{
}

StdCppThreadFactory::StdCppThread* StdCppThreadFactory::createStdCppThread(OSWrapper::Runnable* r, int priority, std::size_t stackSize, const char* name)
{
	return new StdCppThread(r, priority, stackSize, name);
}

OSWrapper::Thread* StdCppThreadFactory::create(OSWrapper::Runnable* r, int priority, std::size_t stackSize, void* stackAddress, const char* name)
{
	(void)stackAddress;
	StdCppThread* t = nullptr;
	try {
		t = createStdCppThread(r, priority, stackSize, name);
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

void StdCppThreadFactory::destroy(OSWrapper::Thread* t)
{
	StdCppThread* stdThread = static_cast<StdCppThread*>(t);
	stdThread->endThread();
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	m_threadIdMap.erase(stdThread->getId());
	delete stdThread;
}

void StdCppThreadFactory::sleep(unsigned long millis)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(millis));
}

void StdCppThreadFactory::yield()
{
	std::this_thread::yield();
}

OSWrapper::Thread* StdCppThreadFactory::getCurrentThread()
{
	std::lock_guard<std::recursive_mutex> lock(m_mutex);
	auto iter = m_threadIdMap.find(std::this_thread::get_id());
	if (iter == m_threadIdMap.end()) {
		return nullptr;
	}
	return iter->second;
}

int StdCppThreadFactory::getMaxPriority() const
{
	return 0;
}

int StdCppThreadFactory::getMinPriority() const
{
	return 0;
}

int StdCppThreadFactory::getHighestPriority() const
{
	return 0;
}

int StdCppThreadFactory::getLowestPriority() const
{
	return 0;
}

}
