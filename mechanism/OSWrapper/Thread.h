#ifndef OS_WRAPPER_THREAD_H_INCLUDED
#define OS_WRAPPER_THREAD_H_INCLUDED

#include <cstddef>
#include "Timeout.h"
#include "OSWrapperError.h"

namespace OSWrapper {

class Runnable;
class ThreadFactory;

/*!
 * @brief Register the ThreadFactory
 * @param factory Pointer of the object of the concrete class derived from ThreadFactory
 *
 * @note You need to call this function only one time when the application is initialized.
 */
void registerThreadFactory(ThreadFactory* factory);

/*!
 * @brief Abstract class that has functions of common RTOS's thread
 *
 * This is the state machine diagram of Thread.
 *
 * @startuml
 *
 * title Thread State Transition
 *
 * [*] -right-> FINISHED : create
 * FINISHED -right-> READY : start
 * READY --> RUNNING : dispatch
 * RUNNING --> READY : preempted
 * RUNNING -right-> WAITING : wait
 * WAITING --> READY : release from waiting
 * RUNNING --> FINISHED : finish
 * FINISHED --> [*] : destroy
 *
 * @enduml
 *
 * When the current thread calls some methods (for example Thread::wait(), Mutex::lock(), EventFlag::timedWait(), Thread::sleep(), etc),
 * the state of current thread transits from RUNNING to WAITING.
 * Then the waiting condition is satisfied or timed out, the state transits from WAITING to READY.
 *
 * When the current thread runs through the end of Runnable::run() or calls Thread::exit(),
 * the state of current thread transits from RUNNING to FINISHED.
 *
 * @note A Thread can not suspend, resume and stop other Thread.
 */
class Thread {
protected:
	Thread(Runnable* r)
	: m_runnable(r), m_uncaughtExceptionHandler(0) {}
	virtual ~Thread() {}

	void threadMain();

public:
	/*!
	 * @brief Interface for handling uncaught exception
	 *
	 * If an exception is not caught in Runnable::run(),
	 * Thread object catches the exception not to terminate the application
	 * and the class that implements this interface handles the exception.
	 */
	class UncaughtExceptionHandler {
	public:
		virtual ~UncaughtExceptionHandler() {}
		/*!
		 * @brief Handle uncaught exception thrown in the Thread
		 * @param t Pointer of Thread object that occurred the exception
		 * @param msg Message of the exception
		 */
		virtual void handle(Thread* t, const char* msg) = 0;
	};

	static const int INHERIT_PRIORITY; //!< Use when set the same priority as the caller thread

	/*!
	 * @brief Set the default UncaughtExceptionHandler for all the Thread
	 * @param handler Pointer of UncaughtExceptionHandler object
	 */
	static void setDefaultUncaughtExceptionHandler(UncaughtExceptionHandler* handler);

	/*!
	 * @brief Get the default UncaughtExceptionHandler
	 * @return The default UncaughtExceptionHandler object
	 */
	static UncaughtExceptionHandler* getDefaultUncaughtExceptionHandler();

	/*!
	 * @brief Create a Thread object
	 * @param r Pointer of Runnable object
	 * @param priority Thread priority. If INHERIT_PRIORITY then sets the priority of the caller thread.
	 * @param stackSize Stack size of thread. If zero then sets default stack size of RTOS.
	 * @param stackAddress Stack address of thread allocated by the caller. If null pointer then RTOS allocates the stack.
	 * @param name Name of the object
	 * @return If this method succeeds then returns a pointer of Thread object, else returns null pointer
	 *
	 * @note Created Thread object is in the FINISHED state. You need to call start() for the Thread object.
	 */
	static Thread* create(Runnable* r, int priority = INHERIT_PRIORITY, std::size_t stackSize = 0U, void* stackAddress = 0, const char* name = "");

	/*!
	 * @brief Destroy a Thread object
	 * @param t Pointer of Thread object created by Thread::create()
	 *
	 * @note If t is null pointer, do nothing.
	 */
	static void destroy(Thread* t);

	/*!
	 * @brief Sleep the current thread while the time that is set by argument
	 * @param millis Time in milliseconds
	 */
	static void sleep(unsigned long millis);

	/*!
	 * @brief Transit the current thread from the RUNNING state to the READY state to give other thread the RUNNING state
	 */
	static void yield();

	/*!
	 * @brief Get the current running Thread object
	 * @return Pointer of the current Thread object
	 *
	 * @attention If current running thread is not created by Thread::create(), returns null pointer.
	 */
	static Thread* getCurrentThread();

	/*!
	 * @brief Get the maximum value of RTOS's thread priority
	 * @return The maximum value of thread priority
	 *
	 * @note Thread::getMinPriority() < Thread::getNormalPriority() < Thread::getMaxPriority()
	 */
	static int getMaxPriority();

	/*!
	 * @brief Get the minimum value of RTOS's thread priority
	 * @return The minimum value of thread priority
	 *
	 * @note Thread::getMinPriority() < Thread::getNormalPriority() < Thread::getMaxPriority()
	 */
	static int getMinPriority();

	/*!
	 * @brief Get the middle priority between the minimum and maximum value
	 * @return The middle priority between the minimum and maximum value
	 *
	 * @note Thread::getMinPriority() < Thread::getNormalPriority() < Thread::getMaxPriority()
	 */
	static int getNormalPriority();

	/*!
	 * @brief Get the highest thread priority of RTOS
	 * @return The highest thread priority
	 *
	 * @note Return value may be the maximum or minimum value by kinds of RTOS.
	 */
	static int getHighestPriority();

	/*!
	 * @brief Get the lowest thread priority of RTOS
	 * @return The lowest thread priority
	 *
	 * @note Return value may be the maximum or minimum value by kinds of RTOS.
	 */
	static int getLowestPriority();

	/*!
	 * @brief Get the thread priority of RTOS higher than \p basePriority by \p d
	 * @param basePriority Base priority
	 * @param d Difference
	 * @return If \p d is positive number, returns the priority higher than \p basePriority by \p d.
	 * @return If \p d is negative number, returns the priority lower than \p basePriority by \p d.
	 * @return If \p d is zero, returns \p basePriority.
	 *
	 * @note The range of return value is between getLowestPriority() and getHighestPriority().
	 */
	static int getPriorityHigherThan(int basePriority, int d);

	/*!
	 * @brief Transit this thread to the READY state
	 *
	 * After this method, when this thread transits to the RUNNING state, this thread calls Runnable::run() specified at create().
	 *
	 * @note If this thread is already in the READY state, do nothing.
	 * @note If this thread is finished, you can start() again.
	 */
	virtual void start() = 0;

	/*!
	 * @brief Block the current thread until this thread transits to the FINISHED state
	 * @retval OK Success. This thread is in the FINISHED state
	 * @retval CalledByNonThread Called from non thread context (interrupt handler, timer, etc)
	 *
	 * @note Same as timedWait(Timeout::FOREVER)
	 */
	virtual Error wait() = 0;

	/*!
	 * @brief Query without blocking whether this thread is in the FINISHED state
	 * @retval OK This thread is already in the FINISHED state
	 * @retval TimedOut This thread is not in the FINISHED state
	 *
	 * @note Same as timedWait(Timeout::POLLING)
	 */
	virtual Error tryWait() = 0;

	/*!
	 * @brief Block the current thread until this thread transits to the FINISHED state but only within the limited time
	 * @param tmout The limited time
	 * @retval OK Success. This thread is in the FINISHED state
	 * @retval TimedOut The limited time was elapsed
	 * @retval CalledByNonThread Called from non thread context (interrupt handler, timer, etc)
	 *
	 * @note If tmout is Timeout::POLLING then this method queries the state without blocking.
	 * @note If tmout is Timeout::FOREVER then this method waits forever until the condition is satisfied.
	 */
	virtual Error timedWait(Timeout tmout) = 0;

	/*!
	 * @brief Return true if this thread is in the FINISHED state
	 * @retval true This thread is in the FINISHED state
	 * @retval false This thread is not in the FINISHED state
	 */
	virtual bool isFinished() const = 0;

	/*!
	 * @brief Set the object's name
	 * @param name The object's name
	 */
	virtual void setName(const char* name) = 0;

	/*!
	 * @brief Get the object's name
	 * @return The object's name
	 */
	virtual const char* getName() const = 0;

	/*!
	 * @brief Change this thread priority
	 * @param priority Thread priority
	 */
	virtual void setPriority(int priority) = 0;

	/*!
	 * @brief Get this thread priority
	 * @return Thread priority
	 */
	virtual int getPriority() const = 0;

	/*!
	 * @brief Get this thread priority when this thread was created
	 * @return Initial thread priority
	 */
	virtual int getInitialPriority() const = 0;

	/*!
	 * @brief Get the stack size of this thread
	 * @return stack size
	 */
	virtual std::size_t getStackSize() const = 0;

	/*!
	 * @brief Get the native handle of RTOS's thread
	 * @return native handle
	 */
	virtual void* getNativeHandle() = 0;

	/*!
	 * @brief Set the UncaughtExceptionHandler for this Thread
	 * @param handler Pointer of UncaughtExceptionHandler object
	 *
	 * @note If set UncaughtExceptionHandler by this method then
	 *       the default UncaughtExceptionHandler is not called when an exception is not caught.
	 */
	void setUncaughtExceptionHandler(UncaughtExceptionHandler* handler);

	/*!
	 * @brief Get the UncaughtExceptionHandler for this Thread
	 * @return The UncaughtExceptionHandler object
	 */
	UncaughtExceptionHandler* getUncaughtExceptionHandler() const;

private:
	Runnable* m_runnable;
	UncaughtExceptionHandler* m_uncaughtExceptionHandler;
	static UncaughtExceptionHandler* m_defaultUncaughtExceptionHandler;

	void handleException(const char* msg);

};

}

#endif // OS_WRAPPER_THREAD_H_INCLUDED
