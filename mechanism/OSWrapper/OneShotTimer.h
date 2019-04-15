#ifndef OS_WRAPPER_ONESHOT_TIMER_H_INCLUDED
#define OS_WRAPPER_ONESHOT_TIMER_H_INCLUDED

namespace OSWrapper {

class Runnable;
class OneShotTimerFactory;

void registerOneShotTimerFactory(OneShotTimerFactory* factory);

class OneShotTimer {
protected:
	OneShotTimer(Runnable* r)
	: m_runnable(r), m_uncaughtExceptionHandler(0) {}
	virtual ~OneShotTimer() {}

	void timerMain();

public:
	class UncaughtExceptionHandler {
	public:
		virtual ~UncaughtExceptionHandler() {}
		virtual void handle(OneShotTimer* t, const char* msg) = 0;
	};

	static void setDefaultUncaughtExceptionHandler(UncaughtExceptionHandler* handler);
	static UncaughtExceptionHandler* getDefaultUncaughtExceptionHandler();

	static OneShotTimer* create(Runnable* r, const char* name = "");
	static void destroy(OneShotTimer* t);

	virtual void start(unsigned long timeInMillis) = 0;
	virtual void stop() = 0;
	virtual bool isStarted() const = 0;

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

#endif // OS_WRAPPER_ONESHOT_TIMER_H_INCLUDED
