#ifndef OS_WRAPPER_THREAD_POOL_H_INCLUDED
#define OS_WRAPPER_THREAD_POOL_H_INCLUDED

#include <cstddef>
#include "Runnable.h"
#include "Thread.h"
#include "Container/PreallocatedVector.h"

namespace OSWrapper {

template <typename T> class MessageQueue;
class EventFlag;
class FixedMemoryPool;

/*!
 * @brief Class of thread pool pattern
 *
 * ThreadPool creates some threads to call tasks asynchronously.
 * When asynchronous tasks are finished, used threads are reused later instead of destroyed.
 * An asynchronous task needs to be a class that implements OSWrapper::Runnable interface.
 * You can set a thread priority of the asynchronous task when you call start().
 *
 * If you choose to use a WaitGuard when you call start(), you need to wait for the asynchronous task finished.
 * After you check the task finished, you can take data from the task object without mutex.
 *
 * If you choose not to use a WaitGuard when you call start(), you don't have to wait for the task finished.
 * But the destruction of the task object must be later than the task finished.
 * Otherwise the access violation may occur that the thread may access to the destroyed object.
 */
class ThreadPool {
private:
	class TaskRunner : public Runnable {
	public:
		void release();
		Error timedWait(Timeout tmout);

	private:
		friend class ThreadPool;

		TaskRunner(EventFlag* ev, ThreadPool* tp);
		~TaskRunner();
		void startThread(Runnable* task, int priority, bool needsWaiting);
		void run();
		void invoke();
		void afterInvoke();
		void notifyFinished();
		void setThread(Thread* t) { m_thread = t; }
		Thread* getThread() const { return m_thread; }
		EventFlag* getEventFlag() const { return m_ev; }

		Runnable* m_task;
		Thread* m_thread;
		EventFlag* m_ev;
		bool m_needsWaiting;
		ThreadPool* m_tp;

		TaskRunner(const TaskRunner&);
		TaskRunner& operator=(const TaskRunner&);
	};

public:
	/*!
	 * @brief Class to wait for the asynchronous task finished
	 *
	 * WaitGuard has a validity state, valid or invalid.
	 * After constuction, the state is invalid.
	 * The state becomes valid by ThreadPool::start(), ThreadPool::tryStart(), or ThreadPool::timedStart().
	 * If the state is valid, the thread that owns this WaitGuard must wait() until the started asynchronous task finished and must release() the WaitGuard.
	 * After release(), the state becomes invalid.
	 * 
	 * WaitGuard can't be copied.
	 */
	class WaitGuard {
	public:
		/*!
		 * @brief Constructor of WaitGuard
		 * @post isValid() == false
		 */
		WaitGuard();

		/*!
		 * @brief Destructor of WaitGuard
		 *
		 * Destructor calls release() if the validity state is valid.
		 */
		~WaitGuard();

		/*!
		 * @brief Call wait(), release the resource, and turn the valid state to invalid
		 * @post isValid() == false
		 * 
		 * @note If the state is invalid, this method does nothing.
		 */
		void release();

		/*!
		 * @brief Block the current thread until the asynchronous task finished
		 * @retval OK Success. The asynchronous task has finished
		 * @retval CalledByNonThread Called from non thread context (interrupt handler, timer, etc)
		 *
		 * @note If the state is invalid, this method returns OK immediately.
		 * @note Same as timedWait(Timeout::FOREVER)
		 */
		Error wait();

		/*!
		 * @brief Query without blocking whether the asynchronous task finished
		 * @retval OK The asynchronous task has already finished
		 * @retval TimedOut The asynchronous task has not finished yet
		 *
		 * @note If the state is invalid, this method returns OK immediately.
		 * @note Same as timedWait(Timeout::POLLING)
		 */
		Error tryWait();

		/*!
		 * @brief Block the current thread until the asynchronous task finished but only within the limited time
		 * @param tmout The limited time
		 * @retval OK Success. The asynchronous task has finished
		 * @retval TimedOut The limited time was elapsed
		 * @retval CalledByNonThread Called from non thread context (interrupt handler, timer, etc)
		 *
		 * @note If the state is invalid, this method returns OK immediately.
		 * @note If \p tmout is Timeout::POLLING then this method queries the state without blocking.
		 * @note If \p tmout is Timeout::FOREVER then this method waits forever until the condition is satisfied.
		 */
		Error timedWait(Timeout tmout);

		/*!
		 * @brief Return true if the validity state is valid
		 * @retval true The state is valid
		 * @retval false The state is invalid
		 */
		bool isValid() const;

	private:
		friend class ThreadPool;
		TaskRunner* m_runner;

		WaitGuard(const WaitGuard&);
		WaitGuard& operator=(const WaitGuard&);
	};

	/*!
	 * @brief Create a ThreadPool object
	 * @param maxThreads Number of max threads that can execute concurrently
	 * @param stackSize Stack size of threads. If zero then sets default stack size of RTOS.
	 * @param defaultPriority First, the thread priority is \p defaultPriority when the threads are created. Then the priority is changed by the parameter of start() when an asynchronous task is started. After the asynchronous task finishes, the priority returns to \p defaultPriority.
	 * @param threadName Name of threads. Threads are named the same name.
	 * @return If this method succeeds then returns a pointer of ThreadPool object, else returns null pointer
	 */
	static ThreadPool* create(std::size_t maxThreads, std::size_t stackSize = 0U, int defaultPriority = Thread::getNormalPriority(), const char* threadName = "");

	/*!
	 * @brief Destroy a ThreadPool object
	 *
	 * This method waits for all of the asynchronous tasks finished and destroys all of the threads.
	 *
	 * @param p Pointer of ThreadPool object created by ThreadPool::create()
	 *
	 * @note If p is null pointer, do nothing.
	 */
	static void destroy(ThreadPool* p);

	/*!
	 * @brief Start an asynchronous task
	 * @param task Asynchronous task
	 * @param waiter Pointer of variable of WaitGuard object that state is invalid. If this method succeeds, the WaitGuard object becomes valid. If you don't want to wait for the task finished, specify null pointer or omit this.
	 * @param priority Thread priority of the asynchronous task
	 * @retval OK Success. A free thread has been allocated and started
	 * @retval InvalidParameter \p task is null pointer
	 * @retval CalledByNonThread Called from non thread context (interrupt handler, timer, etc)
	 *
	 * @note Same as timedStart(task, Timeout::FOREVER, waiter, priority)
	 */
	Error start(Runnable* task, WaitGuard* waiter = 0, int priority = Thread::INHERIT_PRIORITY);

	/*!
	 * @brief Try to start an asynchronous task without blocking
	 * @param task Asynchronous task
	 * @param waiter Pointer of variable of WaitGuard object that state is invalid. If this method succeeds, the WaitGuard object becomes valid. If you don't want to wait for the task finished, specify null pointer or omit this.
	 * @param priority Thread priority of the asynchronous task
	 * @retval OK Success. A free thread has been allocated and started
	 * @retval InvalidParameter \p task is null pointer
	 * @retval TimedOut Failed to allocate a free thread
	 *
	 * @note Same as timedStart(task, Timeout::POLLING, waiter, priority)
	 */
	Error tryStart(Runnable* task, WaitGuard* waiter = 0, int priority = Thread::INHERIT_PRIORITY);

	/*!
	 * @brief Start an asynchronous task with Timeout
	 *
	 * If all thread of this ThreadPool have already started, this method blocks the current thread for the limited time until a free thread is allocated.
	 *
	 * @param task Asynchronous task
	 * @param tmout The limited time
	 * @param waiter Pointer of variable of WaitGuard object that state is invalid. If this method succeeds, the WaitGuard object becomes valid. If you don't want to wait for the task finished, specify null pointer or omit this.
	 * @param priority Thread priority of the asynchronous task
	 * @retval OK Success. A free thread has been allocated and started
	 * @retval InvalidParameter \p task is null pointer
	 * @retval TimedOut The limited time was elapsed
	 * @retval CalledByNonThread Called from non thread context (interrupt handler, timer, etc)
	 *
	 * @note If \p tmout is Timeout::POLLING then this method does not block.
	 * @note If \p tmout is Timeout::FOREVER then this method waits forever until a free thread is allocated.
	 */
	Error timedStart(Runnable* task, Timeout tmout, WaitGuard* waiter = 0, int priority = Thread::INHERIT_PRIORITY);

	/*!
	 * @brief Set the UncaughtExceptionHandler for this ThreadPool
	 * @param handler Pointer of UncaughtExceptionHandler object
	 */
	void setUncaughtExceptionHandler(Thread::UncaughtExceptionHandler* handler);

	/*!
	 * @brief Get the UncaughtExceptionHandler for this ThreadPool
	 * @return The UncaughtExceptionHandler object
	 */
	Thread::UncaughtExceptionHandler* getUncaughtExceptionHandler() const;

	/*!
	 * @brief Get the name of threads
	 * @return The name of threads
	 */
	const char* getThreadName() const;

private:
	typedef MessageQueue<TaskRunner*> TaskRunnerMQ;
	TaskRunnerMQ* m_freeRunnerQueue;

	typedef Container::PreallocatedVector<Thread*> ThreadVector;
	ThreadVector m_threads;
	void* m_threadMemory;

	FixedMemoryPool* m_threadMemoryPool;
	FixedMemoryPool* m_runnerPool;
	FixedMemoryPool* m_objPool;

	Thread::UncaughtExceptionHandler* m_uncaughtExceptionHandler;
	const int m_defaultPriority;
	const char* m_threadName;

	bool constructMembers(std::size_t maxThreads, std::size_t stackSize);
	void destroyMembers();
	void destroyRunners();
	void destroyThreads();
	void waitThreads();
	void releaseRunner(TaskRunner* runner);
	int getDefaultPriority() const { return m_defaultPriority; }

	ThreadPool(FixedMemoryPool* objPool, int defaultPriority, const char* threadName);
	~ThreadPool();

	ThreadPool(const ThreadPool&);
	ThreadPool& operator=(const ThreadPool&);
};

}

#endif // OS_WRAPPER_THREAD_POOL_H_INCLUDED
