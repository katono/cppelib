#ifndef OS_WRAPPER_PERIODIC_TIMER_H_INCLUDED
#define OS_WRAPPER_PERIODIC_TIMER_H_INCLUDED

namespace OSWrapper {

class Runnable;
class PeriodicTimerFactory;

/*!
 * @brief Register the PeriodicTimerFactory
 * @param factory Pointer of the object of the concrete class derived from PeriodicTimerFactory
 *
 * @note You need to call this function only one time when the application is initialized.
 */
void registerPeriodicTimerFactory(PeriodicTimerFactory* factory);

/*!
 * @brief Abstract class that has functions of common RTOS's periodic timer
 */
class PeriodicTimer {
protected:
	PeriodicTimer(Runnable* r)
	: m_runnable(r), m_uncaughtExceptionHandler(0) {}
	virtual ~PeriodicTimer() {}

	void timerMain();

public:
	/*!
	 * @brief Interface for handling uncaught exception
	 *
	 * If an exception is not caught in Runnable::run(),
	 * PeriodicTimer object catches the exception not to terminate the application
	 * and the class that implements this interface handles the exception.
	 */
	class UncaughtExceptionHandler {
	public:
		virtual ~UncaughtExceptionHandler() {}
		/*!
		 * @brief Handle uncaught exception thrown in the PeriodicTimer
		 * @param t Pointer of PeriodicTimer object that occurred the exception
		 * @param msg Message of the exception
		 */
		virtual void handle(PeriodicTimer* t, const char* msg) = 0;
	};

	/*!
	 * @brief Set the default UncaughtExceptionHandler for all the PeriodicTimer
	 * @param handler Pointer of UncaughtExceptionHandler object
	 */
	static void setDefaultUncaughtExceptionHandler(UncaughtExceptionHandler* handler);

	/*!
	 * @brief Get the default UncaughtExceptionHandler
	 * @return The default UncaughtExceptionHandler object
	 */
	static UncaughtExceptionHandler* getDefaultUncaughtExceptionHandler();

	/*!
	 * @brief Create a PeriodicTimer object
	 * @param r Pointer of Runnable object
	 * @param periodInMillis Timer period in milliseconds
	 * @param name Name of the object
	 * @return If this method succeeds then returns a pointer of PeriodicTimer object, else returns null pointer
	 *
	 * @note Created PeriodicTimer object is stopped. You need to call start() for the PeriodicTimer object.
	 */
	static PeriodicTimer* create(Runnable* r, unsigned long periodInMillis, const char* name = "");

	/*!
	 * @brief Destroy a PeriodicTimer object
	 * @param t Pointer of PeriodicTimer object created by PeriodicTimer::create()
	 *
	 * @note If t is null pointer, do nothing.
	 */
	static void destroy(PeriodicTimer* t);

	/*!
	 * @brief Start the timer
	 *
	 * When the timer period elapses, the timer calls Runnable::run() specified at create().
	 * The timer calls the Runnable::run() per period.
	 *
	 * @note If the timer is already started, do nothing.
	 * @note If the timer is stopped, you can start() again.
	 */
	virtual void start() = 0;

	/*!
	 * @brief Stop the timer
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
	 * @brief Get the timer period (in milliseconds)
	 * @return Timer period (in milliseconds)
	 */
	virtual unsigned long getPeriodInMillis() const = 0;

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
	 * @brief Set the UncaughtExceptionHandler for this PeriodicTimer
	 * @param handler Pointer of UncaughtExceptionHandler object
	 *
	 * @note If set UncaughtExceptionHandler by this method then
	 *       the default UncaughtExceptionHandler is not called when an exception is not caught.
	 */
	void setUncaughtExceptionHandler(UncaughtExceptionHandler* handler);

	/*!
	 * @brief Get the UncaughtExceptionHandler for this PeriodicTimer
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

#endif // OS_WRAPPER_PERIODIC_TIMER_H_INCLUDED
