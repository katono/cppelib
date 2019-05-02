#ifndef OS_WRAPPER_MUTEX_H_INCLUDED
#define OS_WRAPPER_MUTEX_H_INCLUDED

#include "Timeout.h"
#include "OSWrapperError.h"

namespace OSWrapper {

class MutexFactory;

/*!
 * @brief Register the MutexFactory
 * @param factory Pointer of the object of the concrete class derived from MutexFactory
 *
 * @note You need to call this function only one time when the application is initialized.
 */
void registerMutexFactory(MutexFactory* factory);

/*!
 * @brief Abstract class that has functions of common RTOS's mutex
 */
class Mutex {
protected:
	virtual ~Mutex() {}

public:
	/*!
	 * @brief Create a Mutex object
	 * @return If this method succeeds then returns a pointer of Mutex object, else returns null pointer
	 *
	 * @note Use priority inheritance if RTOS's mutex supports.
	 */
	static Mutex* create();

	/*!
	 * @brief Create a Mutex object with priority ceiling protocol
	 * @param priorityCeiling priority ceiling
	 * @return If this method succeeds then returns a pointer of Mutex object, else returns null pointer
	 *
	 * @note If RTOS's mutex does not support priority ceiling protocol, priorityCeiling is ignored.
	 */
	static Mutex* create(int priorityCeiling);

	/*!
	 * @brief Destroy a Mutex object
	 * @param m Pointer of Mutex object created by Mutex::create()
	 *
	 * @note If m is null pointer, do nothing.
	 */
	static void destroy(Mutex* m);

	/*!
	 * @brief Block the current thread until locks this mutex
	 * @retval OK Success. The current thread locked this mutex
	 * @retval CalledByNonThread Called from non thread context (interrupt handler, timer, etc)
	 *
	 * @note Same as timedLock(Timeout::FOREVER)
	 */
	virtual Error lock() = 0;

	/*!
	 * @brief Try to lock this mutex without blocking
	 * @retval OK Success. The current thread locked this mutex
	 * @retval TimedOut Failed. Other thread already locked this mutex
	 *
	 * @note Same as timedLock(Timeout::POLLING)
	 */
	virtual Error tryLock() = 0;

	/*!
	 * @brief Block the current thread until locks this mutex but only within the limited time
	 * @param tmout The limited time
	 * @retval OK Success. The current thread locked this mutex
	 * @retval TimedOut The limited time was elapsed
	 * @retval CalledByNonThread Called from non thread context (interrupt handler, timer, etc)
	 *
	 * @note If tmout is Timeout::POLLING then this method tries to lock this mutex without blocking.
	 * @note If tmout is Timeout::FOREVER then this method waits forever until locks this mutex.
	 */
	virtual Error timedLock(Timeout tmout) = 0;

	/*!
	 * @brief Unlock this mutex that the current thread locks
	 * @retval OK Success. The current thread unlocked this mutex
	 * @retval NotLocked The current thread has not locked this mutex
	 */
	virtual Error unlock() = 0;

};


/*!
 * @brief RAII wrapper of Mutex
 *
 * A LockGuard object is used as a local variable.
 * While the LockGuard object exists in the scope, the current thread is locking the Mutex.
 * When it goes out of the scope and the LockGuard object ends its life, the Mutex is unlocked automatically.
 */
class LockGuard {
public:
	struct AdoptLock {};
	static const AdoptLock ADOPT_LOCK; //!< Used as argument of LockGuard's constructor

	/*!
	 * @brief Constructor of LockGuard with automatic mutex locking
	 * @param m Mutex object that the current thread has not locked
	 *
	 * This constructor calls lock() for the Mutex object.
	 */
	explicit LockGuard(Mutex* m);

	/*!
	 * @brief Constructor of LockGuard using locked Mutex object
	 * @param m Mutex object that the current thread has already locked by the lock methods, for example tryLock() or timedLock()
	 * @param AdoptLock Specify LockGuard::ADOPT_LOCK
	 */
	LockGuard(Mutex* m, AdoptLock);

	/*!
	 * @brief Destructor of LockGuard
	 *
	 * Destructor calls unlock() for the locked Mutex object.
	 */
	~LockGuard();

private:
	Mutex* m_mutex;
	Error m_lockErr;

	LockGuard(const LockGuard&);
	LockGuard& operator=(const LockGuard&);
};

}

#endif // OS_WRAPPER_MUTEX_H_INCLUDED
