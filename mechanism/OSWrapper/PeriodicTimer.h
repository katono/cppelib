#ifndef OS_WRAPPER_PERIODIC_TIMER_H_INCLUDED
#define OS_WRAPPER_PERIODIC_TIMER_H_INCLUDED

namespace OSWrapper {

class Runnable;
class PeriodicTimerFactory;

void registerPeriodicTimerFactory(PeriodicTimerFactory* factory);

class PeriodicTimer {
protected:
	PeriodicTimer(Runnable* r)
	: m_runnable(r), m_uncaughtExceptionHandler(0) {}
	virtual ~PeriodicTimer() {}

	void timerMain();

public:
	class UncaughtExceptionHandler {
	public:
		virtual ~UncaughtExceptionHandler() {}
		virtual void handle(PeriodicTimer* t, const char* msg) = 0;
	};

	static void setDefaultUncaughtExceptionHandler(UncaughtExceptionHandler* handler);
	static UncaughtExceptionHandler* getDefaultUncaughtExceptionHandler();

	static PeriodicTimer* create(Runnable* r, unsigned long periodInMillis, const char* name = "");
	static void destroy(PeriodicTimer* t);

	virtual void start() = 0;
	virtual void stop() = 0;
	virtual bool isStarted() const = 0;
	virtual unsigned long getPeriodInMillis() const = 0;

	virtual void setName(const char* name) = 0;
	virtual const char* getName() const = 0;

	void setUncaughtExceptionHandler(UncaughtExceptionHandler* handler);
	UncaughtExceptionHandler* getUncaughtExceptionHandler() const;

private:
	Runnable* m_runnable;
	UncaughtExceptionHandler* m_uncaughtExceptionHandler;
	static UncaughtExceptionHandler* m_defaultUncaughtExceptionHandler;

	void handleException(const char* msg);

};

}

#endif // OS_WRAPPER_PERIODIC_TIMER_H_INCLUDED
