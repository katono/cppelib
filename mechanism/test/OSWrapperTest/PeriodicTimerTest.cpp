#include "OSWrapper/Runnable.h"
#include "OSWrapper/PeriodicTimer.h"
#include "OSWrapper/PeriodicTimerFactory.h"
#include "Assertion/Assertion.h"
#include <stdexcept>
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

namespace PeriodicTimerTest {

using OSWrapper::Runnable;
using OSWrapper::PeriodicTimer;
using OSWrapper::PeriodicTimerFactory;

class TestRunnable : public Runnable {
public:
	void run()
	{
		mock().actualCall("run").onObject(this);
	}
};

class TestPeriodicTimer : public PeriodicTimer {
private:
	unsigned long m_period;
	const char* m_name;
	bool m_started;

public:
	TestPeriodicTimer(Runnable* r, unsigned long periodInMillis, const char* name)
	: PeriodicTimer(r), m_period(periodInMillis), m_name(name)
	, m_started(false)
	{
	}

	~TestPeriodicTimer()
	{
	}

	void start()
	{
		m_started = true;
		timerMain();
	}

	void stop()
	{
		m_started = false;
	}

	bool isStarted() const
	{
		return m_started;
	}

	unsigned long getPeriodInMillis() const
	{
		return m_period;
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

class TestPeriodicTimerFactory : public PeriodicTimerFactory {
private:
	PeriodicTimer* create(Runnable* r, unsigned long periodInMillis, const char* name)
	{
		PeriodicTimer* t = new TestPeriodicTimer(r, periodInMillis, name);
		return t;
	}
	void destroy(PeriodicTimer* t)
	{
		delete static_cast<TestPeriodicTimer*>(t);
	}
};

TEST_GROUP(PeriodicTimerTest) {
	TestPeriodicTimerFactory testFactory;

	void setup()
	{
		OSWrapper::registerPeriodicTimerFactory(&testFactory);
	}
	void teardown()
	{
		mock().checkExpectations();
		mock().clear();
	}
};

TEST(PeriodicTimerTest, create_destroy)
{
	TestRunnable testRun;
	PeriodicTimer* timer = PeriodicTimer::create(&testRun, 100, "TestPeriodicTimer");
	CHECK(timer);
	PeriodicTimer::destroy(timer);
}

TEST(PeriodicTimerTest, create_failed_runnable_nullptr)
{
	PeriodicTimer* timer = PeriodicTimer::create(0, 100, "TestPeriodicTimer");
	CHECK_FALSE(timer);
}

TEST(PeriodicTimerTest, create_failed_period_is_zero)
{
	TestRunnable testRun;
	PeriodicTimer* timer = PeriodicTimer::create(&testRun, 0, "TestPeriodicTimer");
	CHECK_FALSE(timer);
}

TEST(PeriodicTimerTest, destroy_nullptr)
{
	PeriodicTimer::destroy(0);
}

TEST(PeriodicTimerTest, start_isStarted_stop)
{
	TestRunnable testRun;
	PeriodicTimer* timer = PeriodicTimer::create(&testRun, 100);
	CHECK(timer);
	mock().expectOneCall("run").onObject(&testRun);

	CHECK(!timer->isStarted());
	timer->start();
	CHECK(timer->isStarted());
	timer->stop();
	CHECK(!timer->isStarted());

	PeriodicTimer::destroy(timer);
}

TEST(PeriodicTimerTest, getPeriodInMillis)
{
	TestRunnable testRun;
	PeriodicTimer* timer = PeriodicTimer::create(&testRun, 100);
	CHECK(timer);

	LONGS_EQUAL(100, timer->getPeriodInMillis());

	PeriodicTimer::destroy(timer);
}

TEST(PeriodicTimerTest, name)
{
	TestRunnable testRun;
	PeriodicTimer* timer = PeriodicTimer::create(&testRun, 100, "TestPeriodicTimer");
	CHECK(timer);

	STRCMP_EQUAL("TestPeriodicTimer", timer->getName());
	timer->setName("foo");
	STRCMP_EQUAL("foo", timer->getName());
	PeriodicTimer::destroy(timer);
}

#ifndef CPPELIB_NO_EXCEPTIONS
class UnknownExceptionTestRunnable : public Runnable {
public:
	void run()
	{
		throw 1;
	}
};

class UnknownExceptionHandler : public PeriodicTimer::UncaughtExceptionHandler {
public:
	virtual void handle(PeriodicTimer* t, const char* msg)
	{
		mock().actualCall("handle").withParameter("t", t).withParameter("msg", msg).onObject(this);
	}
};

TEST(PeriodicTimerTest, default_handle_unknown_exception)
{
	UnknownExceptionTestRunnable runnable;
	PeriodicTimer* timer = PeriodicTimer::create(&runnable, 100);
	CHECK(timer);

	UnknownExceptionHandler handler;
	PeriodicTimer::UncaughtExceptionHandler* old = PeriodicTimer::getDefaultUncaughtExceptionHandler();
	PeriodicTimer::setDefaultUncaughtExceptionHandler(&handler);
	mock().expectOneCall("handle").withParameter("t", timer).withParameter("msg", "Unknown Exception").onObject(&handler);

	timer->start();
	timer->stop();
	PeriodicTimer::destroy(timer);

	PeriodicTimer::setDefaultUncaughtExceptionHandler(old);
}

TEST(PeriodicTimerTest, handle_unknown_exception)
{
	UnknownExceptionTestRunnable runnable;
	PeriodicTimer* timer = PeriodicTimer::create(&runnable, 100);
	CHECK(timer);
	POINTERS_EQUAL(0, timer->getUncaughtExceptionHandler());

	UnknownExceptionHandler handler;
	timer->setUncaughtExceptionHandler(&handler);
	POINTERS_EQUAL(&handler, timer->getUncaughtExceptionHandler());
	mock().expectOneCall("handle").withParameter("t", timer).withParameter("msg", "Unknown Exception").onObject(&handler);

	timer->start();
	timer->stop();
	PeriodicTimer::destroy(timer);
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

class AssertExceptionHandler : public PeriodicTimer::UncaughtExceptionHandler {
public:
	virtual void handle(PeriodicTimer* t, const char* msg)
	{
		mock().actualCall("handle").withParameter("t", t).onObject(this);
		STRCMP_CONTAINS("CHECK_ASSERT_EXCEPTION_TEST", msg);
	}
};

TEST(PeriodicTimerTest, default_handle_assert_exception)
{
	AssertExceptionTestRunnable runnable;
	PeriodicTimer* timer = PeriodicTimer::create(&runnable, 100);
	CHECK(timer);

	AssertExceptionHandler handler;
	PeriodicTimer::UncaughtExceptionHandler* old = PeriodicTimer::getDefaultUncaughtExceptionHandler();
	PeriodicTimer::setDefaultUncaughtExceptionHandler(&handler);
	mock().expectOneCall("handle").withParameter("t", timer).onObject(&handler);

	timer->start();
	timer->stop();
	PeriodicTimer::destroy(timer);

	PeriodicTimer::setDefaultUncaughtExceptionHandler(old);
}

TEST(PeriodicTimerTest, handle_assert_exception)
{
	AssertExceptionTestRunnable runnable;
	PeriodicTimer* timer = PeriodicTimer::create(&runnable, 100);
	CHECK(timer);
	POINTERS_EQUAL(0, timer->getUncaughtExceptionHandler());

	AssertExceptionHandler handler;
	timer->setUncaughtExceptionHandler(&handler);
	POINTERS_EQUAL(&handler, timer->getUncaughtExceptionHandler());
	mock().expectOneCall("handle").withParameter("t", timer).onObject(&handler);

	timer->start();
	timer->stop();
	PeriodicTimer::destroy(timer);
}

class StdExceptionTestRunnable : public Runnable {
public:
	void run()
	{
		throw std::runtime_error("Exception Test");
	}
};

class StdExceptionHandler : public PeriodicTimer::UncaughtExceptionHandler {
public:
	virtual void handle(PeriodicTimer* t, const char* msg)
	{
		mock().actualCall("handle").withParameter("t", t).onObject(this);
		STRCMP_CONTAINS("Exception Test", msg);
	}
};

TEST(PeriodicTimerTest, default_handle_std_exception)
{
	StdExceptionTestRunnable runnable;
	PeriodicTimer* timer = PeriodicTimer::create(&runnable, 100);
	CHECK(timer);

	StdExceptionHandler handler;
	PeriodicTimer::UncaughtExceptionHandler* old = PeriodicTimer::getDefaultUncaughtExceptionHandler();
	PeriodicTimer::setDefaultUncaughtExceptionHandler(&handler);
	mock().expectOneCall("handle").withParameter("t", timer).onObject(&handler);

	timer->start();
	timer->stop();
	PeriodicTimer::destroy(timer);

	PeriodicTimer::setDefaultUncaughtExceptionHandler(old);
}

TEST(PeriodicTimerTest, handle_std_exception)
{
	StdExceptionTestRunnable runnable;
	PeriodicTimer* timer = PeriodicTimer::create(&runnable, 100);
	CHECK(timer);
	POINTERS_EQUAL(0, timer->getUncaughtExceptionHandler());

	StdExceptionHandler handler;
	timer->setUncaughtExceptionHandler(&handler);
	POINTERS_EQUAL(&handler, timer->getUncaughtExceptionHandler());
	mock().expectOneCall("handle").withParameter("t", timer).onObject(&handler);

	timer->start();
	timer->stop();
	PeriodicTimer::destroy(timer);
}

class ThrowExceptionHandler : public PeriodicTimer::UncaughtExceptionHandler {
public:
	virtual void handle(PeriodicTimer*, const char*)
	{
		throw 0;
	}
};

TEST(PeriodicTimerTest, ExceptionHandler_throws_exception)
{
	StdExceptionTestRunnable runnable;
	PeriodicTimer* timer = PeriodicTimer::create(&runnable, 100);
	CHECK(timer);

	ThrowExceptionHandler handler;
	timer->setUncaughtExceptionHandler(&handler);

	timer->start();
	timer->stop();
	PeriodicTimer::destroy(timer);
}
#endif

} // namespace PeriodicTimerTest
