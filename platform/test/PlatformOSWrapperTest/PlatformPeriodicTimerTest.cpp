#include "OSWrapper/Runnable.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/Mutex.h"
#include "OSWrapper/EventFlag.h"
#include "OSWrapper/PeriodicTimer.h"
#include "Assertion/Assertion.h"
#include <stdexcept>
#include <cstdlib>

#include "PlatformOSWrapperTestHelper.h"

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

namespace PlatformPeriodicTimerTest {

using OSWrapper::Runnable;
using OSWrapper::Thread;
using OSWrapper::Mutex;
using OSWrapper::EventFlag;
using OSWrapper::LockGuard;
using OSWrapper::PeriodicTimer;

static Mutex* s_mutex;

TEST_GROUP(PlatformPeriodicTimerTest) {
	PeriodicTimer* timer;

	void setup()
	{
		PlatformOSWrapperTestHelper::createAndRegisterOSWrapperFactories();
		s_mutex = Mutex::create();
	}
	void teardown()
	{
		Mutex::destroy(s_mutex);
		PlatformOSWrapperTestHelper::destroyOSWrapperFactories();

		mock().checkExpectations();
		mock().clear();
	}
};

class TimerRunnable : public Runnable {
	unsigned long m_period;
	unsigned long m_prevTime;
	std::size_t m_runCount;
public:
	TimerRunnable(unsigned long period) : m_period(period), m_prevTime(0), m_runCount(0) {}
	unsigned long getPeriod() const { return m_period; }
	void setStartTime()
	{
		m_prevTime = PlatformOSWrapperTestHelper::getCurrentTime();
	}
	void run()
	{
		if (m_prevTime == 0) {
			m_prevTime = PlatformOSWrapperTestHelper::getCurrentTime();
			return;
		}
		unsigned long time = PlatformOSWrapperTestHelper::getCurrentTime();
		unsigned long diff = time - m_prevTime;
		unsigned long tolerance = PlatformOSWrapperTestHelper::getTimeTolerance();
		if (std::abs(static_cast<long>(diff) - static_cast<long>(m_period)) > static_cast<long>(tolerance)) {
			LockGuard lock(s_mutex);
			FAIL(StringFromFormat("Check (abs(diff - period) <= tolerance) failed. diff:%ld, period:%ld, tolerance:%ld", diff, m_period, tolerance).asCharString());
		}
		m_prevTime = time;
		LockGuard lock(s_mutex);
		m_runCount++;
	}
	std::size_t getRunCount() const
	{
		LockGuard lock(s_mutex);
		return m_runCount;
	}
};


TEST(PlatformPeriodicTimerTest, create_destroy)
{
	TimerRunnable runnable(100);
	timer = PeriodicTimer::create(&runnable, runnable.getPeriod(), "TestPeriodicTimer");
	CHECK(timer);
	PeriodicTimer::destroy(timer);
}

TEST(PlatformPeriodicTimerTest, create_failed_runnable_nullptr)
{
	timer = PeriodicTimer::create(0, 100, "TestPeriodicTimer");
	CHECK_FALSE(timer);
}

TEST(PlatformPeriodicTimerTest, create_failed_period_zero)
{
	TimerRunnable runnable(100);
	timer = PeriodicTimer::create(&runnable, 0, "TestPeriodicTimer");
	CHECK_FALSE(timer);
}

TEST(PlatformPeriodicTimerTest, getPeriodInMillis)
{
	TimerRunnable runnable(100);
	timer = PeriodicTimer::create(&runnable, runnable.getPeriod(), "TestPeriodicTimer");
	CHECK(timer);
	LONGS_EQUAL(100, timer->getPeriodInMillis());
	PeriodicTimer::destroy(timer);
}

TEST(PlatformPeriodicTimerTest, start_isStarted_stop)
{
	TimerRunnable runnable(100);
	timer = PeriodicTimer::create(&runnable, runnable.getPeriod(), "TestPeriodicTimer");
	CHECK(timer);
	CHECK(!timer->isStarted());
	runnable.setStartTime();
	timer->start();

	CHECK(timer->isStarted());

	Thread::sleep(1000);

	timer->stop();
	CHECK(!timer->isStarted());

	PeriodicTimer::destroy(timer);
	CHECK_COMPARE(9, <=, runnable.getRunCount());
}

TEST(PlatformPeriodicTimerTest, repeat_start_stop)
{
	TimerRunnable runnable(100);
	timer = PeriodicTimer::create(&runnable, runnable.getPeriod(), "TestPeriodicTimer");
	CHECK(timer);
	runnable.setStartTime();
	timer->start();
	timer->start();

	Thread::sleep(200);

	timer->stop();
	timer->stop();

	PeriodicTimer::destroy(timer);
}

TEST(PlatformPeriodicTimerTest, restart)
{
	TimerRunnable runnable(100);
	timer = PeriodicTimer::create(&runnable, runnable.getPeriod(), "TestPeriodicTimer");
	CHECK(timer);
	runnable.setStartTime();

	timer->start();
	Thread::sleep(200);
	timer->stop();

	runnable.setStartTime();

	timer->start();
	Thread::sleep(200);
	timer->stop();

	PeriodicTimer::destroy(timer);
}

TEST(PlatformPeriodicTimerTest, multiple_timers)
{
	TimerRunnable runnable1(100);
	PeriodicTimer* timer1 = PeriodicTimer::create(&runnable1, runnable1.getPeriod(), "TestPeriodicTimer1");
	CHECK(timer1);

	TimerRunnable runnable2(200);
	PeriodicTimer* timer2 = PeriodicTimer::create(&runnable2, runnable2.getPeriod(), "TestPeriodicTimer2");
	CHECK(timer2);

	TimerRunnable runnable3(100);
	PeriodicTimer* timer3 = PeriodicTimer::create(&runnable3, runnable3.getPeriod(), "TestPeriodicTimer3");
	CHECK(timer3);

	runnable1.setStartTime();
	timer1->start();
	runnable2.setStartTime();
	timer2->start();
	Thread::sleep(200);

	runnable3.setStartTime();
	timer3->start();
	Thread::sleep(240);

	timer1->stop();
	timer2->stop();
	timer3->stop();

	LONGS_EQUAL(4, runnable1.getRunCount());
	LONGS_EQUAL(2, runnable2.getRunCount());
	LONGS_EQUAL(2, runnable3.getRunCount());

	PeriodicTimer::destroy(timer1);
	PeriodicTimer::destroy(timer2);
	PeriodicTimer::destroy(timer3);
}

TEST(PlatformPeriodicTimerTest, start_stop_continuously_at_once)
{
	TimerRunnable runnable(100);
	timer = PeriodicTimer::create(&runnable, runnable.getPeriod(), "TestPeriodicTimer");
	CHECK(timer);
	runnable.setStartTime();

	timer->start();
	timer->stop();

	runnable.setStartTime();

	timer->start();
	timer->stop();

	runnable.setStartTime();

	timer->start();
	Thread::sleep(200);
	timer->stop();

	PeriodicTimer::destroy(timer);
}

TEST(PlatformPeriodicTimerTest, start_destroy)
{
	TimerRunnable runnable(100);
	timer = PeriodicTimer::create(&runnable, runnable.getPeriod(), "TestPeriodicTimer");
	CHECK(timer);
	runnable.setStartTime();

	timer->start();

	Thread::sleep(100);

	PeriodicTimer::destroy(timer);
}

TEST(PlatformPeriodicTimerTest, start_destroy_at_once)
{
	TimerRunnable runnable(100);
	timer = PeriodicTimer::create(&runnable, runnable.getPeriod(), "TestPeriodicTimer");
	CHECK(timer);
	runnable.setStartTime();

	timer->start();
	PeriodicTimer::destroy(timer);
}

TEST(PlatformPeriodicTimerTest, name)
{
	TimerRunnable runnable(100);
	timer = PeriodicTimer::create(&runnable, runnable.getPeriod(), "TestPeriodicTimer");
	CHECK(timer);

	STRCMP_EQUAL("TestPeriodicTimer", timer->getName());
	timer->setName("foo");
	STRCMP_EQUAL("foo", timer->getName());
	PeriodicTimer::destroy(timer);
}

#ifndef CPPELIB_NO_EXCEPTIONS
class ThrowExceptionRunnable : public Runnable {
public:
	enum Kind {
		Std,
		Assert,
		Integer,
	};
	ThrowExceptionRunnable(Kind kind) : m_kind(kind) {}
	void run()
	{
		switch (m_kind) {
		case Std:
			throw std::runtime_error("Exception Test");
			break;
		case Assert:
			CHECK_ASSERT("CHECK_ASSERT_EXCEPTION_TEST" && false);
			break;
		case Integer:
			throw 1;
			break;
		}
	}
private:
	Kind m_kind;
};

class MyExceptionHandler : public PeriodicTimer::UncaughtExceptionHandler {
	const char* m_expectedMsg;
public:
	explicit MyExceptionHandler(const char* expectedMsg) : m_expectedMsg(expectedMsg) {}
	virtual void handle(PeriodicTimer* t, const char* expectedMsg)
	{
		mock().actualCall("handle").withParameter("t", t).onObject(this);
		STRCMP_CONTAINS(m_expectedMsg, expectedMsg);
	}
};

TEST(PlatformPeriodicTimerTest, exception_std)
{
	ThrowExceptionRunnable runnable(ThrowExceptionRunnable::Std);
	timer = PeriodicTimer::create(&runnable, 100, "TestPeriodicTimer");

	MyExceptionHandler handler("Exception Test");
	timer->setUncaughtExceptionHandler(&handler);
	mock().expectOneCall("handle").withParameter("t", timer).onObject(&handler);

	timer->start();
	Thread::sleep(190);
	timer->stop();

	PeriodicTimer::destroy(timer);
}

TEST(PlatformPeriodicTimerTest, exception_assert)
{
	ThrowExceptionRunnable runnable(ThrowExceptionRunnable::Assert);
	timer = PeriodicTimer::create(&runnable, 100, "TestPeriodicTimer");

	MyExceptionHandler handler("CHECK_ASSERT_EXCEPTION_TEST");
	timer->setUncaughtExceptionHandler(&handler);
	mock().expectOneCall("handle").withParameter("t", timer).onObject(&handler);

	timer->start();
	Thread::sleep(190);
	timer->stop();

	PeriodicTimer::destroy(timer);
}

TEST(PlatformPeriodicTimerTest, exception_unknown)
{
	ThrowExceptionRunnable runnable(ThrowExceptionRunnable::Integer);
	timer = PeriodicTimer::create(&runnable, 100, "TestPeriodicTimer");

	MyExceptionHandler handler("Unknown Exception");
	timer->setUncaughtExceptionHandler(&handler);
	mock().expectOneCall("handle").withParameter("t", timer).onObject(&handler);

	timer->start();
	Thread::sleep(190);
	timer->stop();

	PeriodicTimer::destroy(timer);
}
#endif

} // namespace PlatformPeriodicTimerTest
