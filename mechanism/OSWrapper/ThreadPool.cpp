#include "ThreadPool.h"
#include "MessageQueue.h"
#include "EventFlag.h"
#include "FixedMemoryPool.h"

namespace OSWrapper {

ThreadPool::ThreadPool(FixedMemoryPool* objPool, int defaultPriority, const char* threadName)
: m_freeRunnerQueue(0)
, m_threads()
, m_threadMemory(0)
, m_threadMemoryPool(0)
, m_runnerPool(0)
, m_objPool(objPool)
, m_uncaughtExceptionHandler(0)
, m_defaultPriority(defaultPriority)
, m_threadName(threadName)
{
}

ThreadPool::~ThreadPool()
{
}

ThreadPool* ThreadPool::create(std::size_t maxThreads, std::size_t stackSize/*= 0U*/, int defaultPriority/*= Thread::getNormalPriority()*/, const char* threadName/*= ""*/)
{
	const std::size_t objSize = sizeof(ThreadPool);
	FixedMemoryPool* objPool = FixedMemoryPool::create(objSize,
			FixedMemoryPool::getRequiredMemorySize(objSize, 1U));
	if (objPool == 0) {
		return 0;
	}

	void* p = objPool->allocate();
	if (p == 0) {
		FixedMemoryPool::destroy(objPool);
		return 0;
	}
	ThreadPool* tp = new(p) ThreadPool(objPool, defaultPriority, threadName);

	if (!tp->constructMembers(maxThreads, stackSize)) {
		tp->~ThreadPool();
		objPool->deallocate(tp);
		FixedMemoryPool::destroy(objPool);
		return 0;
	}
	return tp;
}

bool ThreadPool::constructMembers(std::size_t maxThreads, std::size_t stackSize)
{
	const std::size_t threadMemoryPoolSize = sizeof(Thread*) * maxThreads;

	m_freeRunnerQueue = TaskRunnerMQ::create(maxThreads);
	if (m_freeRunnerQueue == 0) {
		goto failed;
	}

	m_threadMemoryPool = FixedMemoryPool::create(threadMemoryPoolSize,
			FixedMemoryPool::getRequiredMemorySize(threadMemoryPoolSize, 1U));
	if (m_threadMemoryPool == 0) {
		goto failed;
	}
	m_threadMemory = m_threadMemoryPool->allocate();
	if (m_threadMemory == 0) {
		goto failed;
	}
	m_threads.init(m_threadMemory, threadMemoryPoolSize);

	m_runnerPool = FixedMemoryPool::create(sizeof(TaskRunner),
			FixedMemoryPool::getRequiredMemorySize(sizeof(TaskRunner), maxThreads));
	if (m_runnerPool == 0) {
		goto failed;
	}

	for (std::size_t i = 0U; i < maxThreads; ++i) {
		void* p = m_runnerPool->allocate();
		if (p == 0) {
			goto failed;
		}

		EventFlag* e = EventFlag::create(false);
		if (e == 0) {
			m_runnerPool->deallocate(p);
			goto failed;
		}

		TaskRunner* runner = new(p) TaskRunner(e, this);

		Thread* t = Thread::create(runner, m_defaultPriority, stackSize, 0, m_threadName);
		if (t == 0) {
			EventFlag::destroy(e);
			runner->~TaskRunner();
			m_runnerPool->deallocate(p);
			goto failed;
		}
		runner->setThread(t);

		m_threads.push_back(t);
		m_freeRunnerQueue->send(runner);
	}
	return true;

failed:
	destroyMembers();
	return false;
}

void ThreadPool::destroy(ThreadPool* p)
{
	if (p == 0) {
		return;
	}
	FixedMemoryPool* objPool = p->m_objPool;
	p->waitThreads();
	p->destroyMembers();
	p->~ThreadPool();
	objPool->deallocate(p);
	FixedMemoryPool::destroy(objPool);
}

void ThreadPool::waitThreads()
{
	for (std::size_t i = 0U; i < m_threads.size(); ++i) {
		m_threads[i]->wait();
	}
}

void ThreadPool::destroyMembers()
{
	destroyRunners();
	destroyThreads();

	FixedMemoryPool::destroy(m_runnerPool);
	FixedMemoryPool::destroy(m_threadMemoryPool);
	TaskRunnerMQ::destroy(m_freeRunnerQueue);
}

void ThreadPool::destroyRunners()
{
	if (m_freeRunnerQueue == 0) {
		return;
	}
	if (m_runnerPool == 0) {
		return;
	}
	while (true) {
		TaskRunner* runner = 0;
		Error err = m_freeRunnerQueue->tryReceive(&runner);
		if (err != OK) {
			// empty
			break;
		}
		EventFlag::destroy(runner->getEventFlag());
		Thread::destroy(runner->getThread());
		runner->~TaskRunner();
		m_runnerPool->deallocate(runner);
	}
}

void ThreadPool::destroyThreads()
{
	if (m_threadMemoryPool == 0) {
		return;
	}
	if (m_threadMemory == 0) {
		return;
	}
	m_threadMemoryPool->deallocate(m_threadMemory);
}

Error ThreadPool::start(Runnable* task, WaitGuard* waiter/*= 0*/, int priority/*= Thread::INHERIT_PRIORITY*/)
{
	return timedStart(task, Timeout::FOREVER, waiter, priority);
}

Error ThreadPool::tryStart(Runnable* task, WaitGuard* waiter/*= 0*/, int priority/*= Thread::INHERIT_PRIORITY*/)
{
	return timedStart(task, Timeout::POLLING, waiter, priority);
}

Error ThreadPool::timedStart(Runnable* task, Timeout tmout, WaitGuard* waiter/*= 0*/, int priority/*= Thread::INHERIT_PRIORITY*/)
{
	if (task == 0) {
		return InvalidParameter;
	}
	TaskRunner* runner = 0;
	const Error recvErr = m_freeRunnerQueue->timedReceive(&runner, tmout);
	if (recvErr != OK) {
		return recvErr;
	}

	Thread* t = runner->getThread();
	const Error waitErr = t->timedWait(tmout);
	if (waitErr != OK) {
		return waitErr;
	}

	runner->startThread(task, priority, (waiter != 0));

	if (waiter != 0) {
		waiter->m_runner = runner;
	}
	return OK;
}

void ThreadPool::releaseRunner(TaskRunner* runner)
{
	m_freeRunnerQueue->send(runner);
}

void ThreadPool::setUncaughtExceptionHandler(Thread::UncaughtExceptionHandler* handler)
{
	m_uncaughtExceptionHandler = handler;
	for (std::size_t i = 0U; i < m_threads.size(); ++i) {
		m_threads[i]->setUncaughtExceptionHandler(handler);
	}
}

Thread::UncaughtExceptionHandler* ThreadPool::getUncaughtExceptionHandler() const
{
	return m_uncaughtExceptionHandler;
}

const char* ThreadPool::getThreadName() const
{
	return m_threadName;
}


ThreadPool::TaskRunner::TaskRunner(EventFlag* ev, ThreadPool* tp)
: m_task(0)
, m_thread(0)
, m_ev(ev)
, m_needsWaiting(false)
, m_tp(tp)
{
}

ThreadPool::TaskRunner::~TaskRunner()
{
}

void ThreadPool::TaskRunner::startThread(Runnable* task, int priority, bool needsWaiting)
{
	m_task = task;
	m_needsWaiting = needsWaiting;
	m_thread->setPriority(priority);
	m_thread->start();
}

void ThreadPool::TaskRunner::run()
{
#ifndef CPPELIB_NO_EXCEPTIONS
	try {
#endif
		invoke();
#ifndef CPPELIB_NO_EXCEPTIONS
	} catch (...) {
		afterInvoke();
		throw;
	}
#endif
	afterInvoke();

}

void ThreadPool::TaskRunner::invoke()
{
	if (m_task != 0) {
		m_task->run();
	}
}

void ThreadPool::TaskRunner::afterInvoke()
{
	m_thread->setPriority(m_tp->getDefaultPriority());
	if (m_needsWaiting) {
		notifyFinished();
	} else {
		release();
	}
}

void ThreadPool::TaskRunner::notifyFinished()
{
	m_ev->setAll();
}

void ThreadPool::TaskRunner::release()
{
	m_ev->resetAll();
	m_tp->releaseRunner(this);
}

Error ThreadPool::TaskRunner::timedWait(Timeout tmout)
{
	return m_ev->timedWaitAny(tmout);
}


ThreadPool::WaitGuard::WaitGuard()
: m_runner(0)
{
}

ThreadPool::WaitGuard::~WaitGuard()
{
	release(); // cppcheck-suppress throwInNoexceptFunction // release() throws no exception
}

void ThreadPool::WaitGuard::release()
{
	if (m_runner != 0) {
		wait();
		m_runner->release();
		m_runner = 0;
	}
}

Error ThreadPool::WaitGuard::wait()
{
	return timedWait(Timeout::FOREVER);
}

Error ThreadPool::WaitGuard::tryWait()
{
	return timedWait(Timeout::POLLING);
}

Error ThreadPool::WaitGuard::timedWait(Timeout tmout)
{
	if (m_runner == 0) {
		return OK;
	}
	return m_runner->timedWait(tmout);
}

bool ThreadPool::WaitGuard::isValid() const
{
	return m_runner != 0;
}

}
