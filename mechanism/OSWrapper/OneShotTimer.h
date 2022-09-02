#ifndef OS_WRAPPER_ONESHOT_TIMER_H_INCLUDED
#define OS_WRAPPER_ONESHOT_TIMER_H_INCLUDED

namespace OSWrapper {

class Runnable;
class OneShotTimerFactory;

/*!
 * @brief Register the OneShotTimerFactory
 * @param factory Pointer of the object of the concrete class derived from OneShotTimerFactory
 *
 * @note You need to call this function only one time when the application is initialized.
 */
void registerOneShotTimerFactory(OneShotTimerFactory* factory);

/*!
 * @brief Abstract class that has functions of common RTOS's one-shot timer
 */
class OneShotTimer {
protected:
	explicit OneShotTimer(Runnable* r)
	: m_runnable(r), m_uncaughtExceptionHandler(0) {}
	virtual ~OneShotTimer() {}

	void timerMain();

public:
	/*!
	 * @brief Interface for handling uncaught exception
	 *
	 * If an exception is not caught in Runnable::run(),
	 * OneShotTimer object catches the exception not to terminate the application
	 * and the class that implements this interface handles the exception.
	 */
	class UncaughtExceptionHandler {
	public:
		virtual ~UncaughtExceptionHandler() {}
		/*!
		 * @brief Handle uncaught exception thrown in the OneShotTimer
		 * @param t Pointer of OneShotTimer object that occurred the exception
		 * @param msg Message of the exception
		 */
		virtual void handle(OneShotTimer* t, const char* msg) = 0;
	};

	/*!
	 * @brief Set the default UncaughtExceptionHandler for all the OneShotTimer
	 * @param handler Pointer of UncaughtExceptionHandler object
	 */
	static void setDefaultUncaughtExceptionHandler(UncaughtExceptionHandler* handler);

	/*!
	 * @brief Get the default UncaughtExceptionHandler
	 * @return The default UncaughtExceptionHandler object
	 */
	static UncaughtExceptionHandler* getDefaultUncaughtExceptionHandler();

	/*!
	 * @brief Create an OneShotTimer object
	 * @param r Pointer of Runnable object
	 * @param name Name of the object
	 * @return If this method succeeds then returns a pointer of OneShotTimer object, else returns null pointer
	 *
	 * @note Created OneShotTimer object is stopped. You need to call start() for the OneShotTimer object.
	 */
	static OneShotTimer* create(Runnable* r, const char* name = "");

	/*!
	 * @brief Destroy an OneShotTimer object
	 * @param t Pointer of OneShotTimer object created by OneShotTimer::create()
	 *
	 * @note If t is null pointer, do nothing.
	 */
	static void destroy(OneShotTimer* t);

	/*!
	 * @brief Start the timer
	 *
	 * After the timeInMillis elapses, the timer calls Runnable::run() specified at create().
	 * When the timer is fired one time, the timer is stopped.
	 *
	 * @param timeInMillis Time in milliseconds until the timer is fired
	 *
	 * @note If the timer is already started, do nothing.
	 * @note If the timer is stopped, you can start() again.
	 */
	virtual void start(unsigned long timeInMillis) = 0;

	/*!
	 * @brief Stop the timer
	 *
	 * Before the timer is fired, it stops the timer.
	 *
	 * @note If the timer is already stopped, do nothing.
	 */
	virtual void stop() = 0;

	/*!
	 * @brief Return true if the timer is started
	 * @retval true The timer is already started
	 * @retval false The timer is already stopped
	 */
	virtual bool isStarted() const = 0;

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
	 * @brief Set the UncaughtExceptionHandler for this OneShotTimer
	 * @param handler Pointer of UncaughtExceptionHandler object
	 *
	 * @note If set UncaughtExceptionHandler by this method then
	 *       the default UncaughtExceptionHandler is not called when an exception is not caught.
	 */
	void setUncaughtExceptionHandler(UncaughtExceptionHandler* handler);

	/*!
	 * @brief Get the UncaughtExceptionHandler for this OneShotTimer
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

#endif // OS_WRAPPER_ONESHOT_TIMER_H_INCLUDED
