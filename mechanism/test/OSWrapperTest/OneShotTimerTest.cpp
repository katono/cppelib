#include "OSWrapper/Runnable.h"
#include "OSWrapper/OneShotTimer.h"
#include "OSWrapper/OneShotTimerFactory.h"
#include "Assertion/Assertion.h"
#include <stdexcept>
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

namespace OneShotTimerTest {

using OSWrapper::Runnable;
using OSWrapper::OneShotTimer;
using OSWrapper::OneShotTimerFactory;

class TestRunnable : public Runnable {
public:
	void run()
	{
		mock().actualCall("run").onObject(this);
	}
};

class TestOneShotTimer : public OneShotTimer {
private:
	unsigned long m_time;
	const char* m_name;
	bool m_started;

public:
	TestOneShotTimer(Runnable* r, const char* name)
	: OneShotTimer(r), m_time(0), m_name(name)
	, m_started(false)
	{
	}

	~TestOneShotTimer()
	{
		if (m_started) {
			timerMain();
		}
	}

	void start(unsigned long timeInMillis)
	{
		(void)timeInMillis;
		m_started = true;
	}

	void stop()
	{
		m_started = false;
	}

	bool isStarted() const
	{
		return m_started;
	}

	void setName(const char* name)
	{
		m_name = name;
	}
	const char* getName() const
	{
		return m_name;
	}

};

class TestOneShotTimerFactory : public OneShotTimerFactory {
private:
	OneShotTimer* create(Runnable* r, const char* name)
	{
		OneShotTimer* t = new TestOneShotTimer(r, name);
		return t;
	}
	void destroy(OneShotTimer* t)
	{
		delete static_cast<TestOneShotTimer*>(t);
	}
};

TEST_GROUP(OneShotTimerTest) {
	TestOneShotTimerFactory testFactory;
	TestRunnable testRun;
	OneShotTimer* timer;

	void setup()
	{
		OSWrapper::registerOneShotTimerFactory(&testFactory);
	}
	void teardown()
	{
		mock().checkExpectations();
		mock().clear();
	}
};

TEST(OneShotTimerTest, create_destroy)
{
	timer = OneShotTimer::create(&testRun, "TestOneShotTimer");
	CHECK(timer);
	OneShotTimer::destroy(timer);
}

TEST(OneShotTimerTest, create_failed_runnable_nullptr)
{
	timer = OneShotTimer::create(0, "TestOneShotTimer");
	CHECK_FALSE(timer);
}

TEST(OneShotTimerTest, destroy_nullptr)
{
	OneShotTimer::destroy(0);
}

TEST(OneShotTimerTest, start_isStarted_stop)
{
	timer = OneShotTimer::create(&testRun);

	CHECK(!timer->isStarted());
	timer->start(100);
	CHECK(timer->isStarted());
	timer->stop();
	CHECK(!timer->isStarted());

	OneShotTimer::destroy(timer);
}

TEST(OneShotTimerTest, start_stop_start)
{
	timer = OneShotTimer::create(&testRun);
	mock().expectOneCall("run").onObject(&testRun);

	CHECK(!timer->isStarted());
	timer->start(100);
	CHECK(timer->isStarted());
	timer->stop();
	CHECK(!timer->isStarted());
	timer->start(100);
	CHECK(timer->isStarted());

	OneShotTimer::destroy(timer);
}

TEST(OneShotTimerTest, name)
{
	timer = OneShotTimer::create(&testRun, "TestOneShotTimer");

	STRCMP_EQUAL("TestOneShotTimer", timer->getName());
	timer->setName("foo");
	STRCMP_EQUAL("foo", timer->getName());
	OneShotTimer::destroy(timer);
}

#ifndef CPPELIB_NO_EXCEPTIONS
class UnknownExceptionTestRunnable : public Runnable {
public:
	void run()
	{
		throw 1;
	}
};

class UnknownExceptionHandler : public OneShotTimer::UncaughtExceptionHandler {
public:
	virtual void handle(OneShotTimer* t, const char* msg)
	{
		mock().actualCall("handle").withParameter("t", t).withParameter("msg", msg).onObject(this);
	}
};

TEST(OneShotTimerTest, default_handle_unknown_exception)
{
	UnknownExceptionTestRunnable runnable;
	timer = OneShotTimer::create(&runnable);

	UnknownExceptionHandler handler;
	OneShotTimer::UncaughtExceptionHandler* old = OneShotTimer::getDefaultUncaughtExceptionHandler();
	OneShotTimer::setDefaultUncaughtExceptionHandler(&handler);
	mock().expectOneCall("handle").withParameter("t", timer).withParameter("msg", "Unknown Exception").onObject(&handler);

	timer->start(100);

	OneShotTimer::destroy(timer);

	OneShotTimer::setDefaultUncaughtExceptionHandler(old);
}

TEST(OneShotTimerTest, handle_unknown_exception)
{
	UnknownExceptionTestRunnable runnable;
	timer = OneShotTimer::create(&runnable);
	POINTERS_EQUAL(0, timer->getUncaughtExceptionHandler());

	UnknownExceptionHandler handler;
	timer->setUncaughtExceptionHandler(&handler);
	POINTERS_EQUAL(&handler, timer->getUncaughtExceptionHandler());
	mock().expectOneCall("handle").withParameter("t", timer).withParameter("msg", "Unknown Exception").onObject(&handler);

	timer->start(100);

	OneShotTimer::destroy(timer);
}

class AssertExceptionTestRunnable : public Runnable {
public:
	void run()
	{
		try {
			CHECK_ASSERT("CHECK_ASSERT_EXCEPTION_TEST" && false);
		}
		catch (const std::exception&) {
			FAIL("NOT std::exception");
		}
	}
};

class AssertExceptionHandler : public OneShotTimer::UncaughtExceptionHandler {
public:
	virtual void handle(OneShotTimer* t, const char* msg)
	{
		mock().actualCall("handle").withParameter("t", t).onObject(this);
		STRCMP_CONTAINS("CHECK_ASSERT_EXCEPTION_TEST", msg);
	}
};

TEST(OneShotTimerTest, default_handle_assert_exception)
{
	AssertExceptionTestRunnable runnable;
	timer = OneShotTimer::create(&runnable);

	AssertExceptionHandler handler;
	OneShotTimer::UncaughtExceptionHandler* old = OneShotTimer::getDefaultUncaughtExceptionHandler();
	OneShotTimer::setDefaultUncaughtExceptionHandler(&handler);
	mock().expectOneCall("handle").withParameter("t", timer).onObject(&handler);

	timer->start(100);

	OneShotTimer::destroy(timer);

	OneShotTimer::setDefaultUncaughtExceptionHandler(old);
}

TEST(OneShotTimerTest, handle_assert_exception)
{
	AssertExceptionTestRunnable runnable;
	timer = OneShotTimer::create(&runnable);
	POINTERS_EQUAL(0, timer->getUncaughtExceptionHandler());

	AssertExceptionHandler handler;
	timer->setUncaughtExceptionHandler(&handler);
	POINTERS_EQUAL(&handler, timer->getUncaughtExceptionHandler());
	mock().expectOneCall("handle").withParameter("t", timer).onObject(&handler);

	timer->start(100);

	OneShotTimer::destroy(timer);
}

class StdExceptionTestRunnable : public Runnable {
public:
	void run()
	{
		throw std::runtime_error("Exception Test");
	}
};

class StdExceptionHandler : public OneShotTimer::UncaughtExceptionHandler {
public:
	virtual void handle(OneShotTimer* t, const char* msg)
	{
		mock().actualCall("handle").withParameter("t", t).onObject(this);
		STRCMP_CONTAINS("Exception Test", msg);
	}
};

TEST(OneShotTimerTest, default_handle_std_exception)
{
	StdExceptionTestRunnable runnable;
	timer = OneShotTimer::create(&runnable);

	StdExceptionHandler handler;
	OneShotTimer::UncaughtExceptionHandler* old = OneShotTimer::getDefaultUncaughtExceptionHandler();
	OneShotTimer::setDefaultUncaughtExceptionHandler(&handler);
	mock().expectOneCall("handle").withParameter("t", timer).onObject(&handler);

	timer->start(100);

	OneShotTimer::destroy(timer);

	OneShotTimer::setDefaultUncaughtExceptionHandler(old);
}

TEST(OneShotTimerTest, handle_std_exception)
{
	StdExceptionTestRunnable runnable;
	timer = OneShotTimer::create(&runnable);
	POINTERS_EQUAL(0, timer->getUncaughtExceptionHandler());

	StdExceptionHandler handler;
	timer->setUncaughtExceptionHandler(&handler);
	POINTERS_EQUAL(&handler, timer->getUncaughtExceptionHandler());
	mock().expectOneCall("handle").withParameter("t", timer).onObject(&handler);

	timer->start(100);

	OneShotTimer::destroy(timer);
}

class ThrowExceptionHandler : public OneShotTimer::UncaughtExceptionHandler {
public:
	virtual void handle(OneShotTimer*, const char*)
	{
		throw 0;
	}
};

TEST(OneShotTimerTest, ExceptionHandler_throws_exception)
{
	StdExceptionTestRunnable runnable;
	timer = OneShotTimer::create(&runnable);

	ThrowExceptionHandler handler;
	timer->setUncaughtExceptionHandler(&handler);

	timer->start(100);

	OneShotTimer::destroy(timer);
}
#endif

} // namespace OneShotTimerTest
