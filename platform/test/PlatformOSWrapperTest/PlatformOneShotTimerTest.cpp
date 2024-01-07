#include "OSWrapper/Runnable.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/Mutex.h"
#include "OSWrapper/EventFlag.h"
#include "OSWrapper/OneShotTimer.h"
#include "Assertion/Assertion.h"
#include <stdexcept>
#include <cstdlib>

#include "PlatformOSWrapperTestHelper.h"

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

namespace PlatformOneShotTimerTest {

using OSWrapper::Runnable;
using OSWrapper::Thread;
using OSWrapper::Mutex;
using OSWrapper::EventFlag;
using OSWrapper::LockGuard;
using OSWrapper::OneShotTimer;

static Mutex* s_mutex;

TEST_GROUP(PlatformOneShotTimerTest) {
	OneShotTimer* timer;

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
	unsigned long m_time;
	unsigned long m_prevTime;
public:
	TimerRunnable() : m_time(0), m_prevTime(0) {}
	void setStartTime(unsigned long time)
	{
		m_time = time;
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
		if (std::abs(static_cast<long>(diff) - static_cast<long>(m_time)) > static_cast<long>(tolerance)) {
			LockGuard lock(s_mutex);
			FAIL(StringFromFormat("Check (abs(diff - timeout) <= tolerance) failed. diff:%ld, timeout:%ld, tolerance:%ld", diff, m_time, tolerance).asCharString());
		}
		m_prevTime = time;
		LockGuard lock(s_mutex);
		mock().actualCall("run").onObject(this);
	}
};


TEST(PlatformOneShotTimerTest, create_destroy)
{
	TimerRunnable runnable;
	timer = OneShotTimer::create(&runnable, "TestOneShotTimer");
	CHECK(timer);
	OneShotTimer::destroy(timer);
}

TEST(PlatformOneShotTimerTest, create_failed_runnable_nullptr)
{
	timer = OneShotTimer::create(0, "TestOneShotTimer");
	CHECK_FALSE(timer);
}

TEST(PlatformOneShotTimerTest, start_isStarted)
{
	TimerRunnable runnable;
	timer = OneShotTimer::create(&runnable, "TestOneShotTimer");
	CHECK(timer);
	mock().expectOneCall("run").onObject(&runnable);

	CHECK(!timer->isStarted());
	runnable.setStartTime(100);
	timer->start(100);

	CHECK(timer->isStarted());

	Thread::sleep(130);

	OneShotTimer::destroy(timer);
}

TEST(PlatformOneShotTimerTest, start_isStarted_stop)
{
	TimerRunnable runnable;
	timer = OneShotTimer::create(&runnable, "TestOneShotTimer");
	CHECK(timer);

	CHECK(!timer->isStarted());
	runnable.setStartTime(100);
	timer->start(100);

	CHECK(timer->isStarted());
	timer->stop();
	CHECK(!timer->isStarted());

	OneShotTimer::destroy(timer);
}

TEST(PlatformOneShotTimerTest, repeat_start)
{
	TimerRunnable runnable;
	timer = OneShotTimer::create(&runnable, "TestOneShotTimer");
	CHECK(timer);
	mock().expectOneCall("run").onObject(&runnable);

	runnable.setStartTime(100);
	timer->start(100);
	timer->start(100);

	Thread::sleep(130);

	OneShotTimer::destroy(timer);
}

TEST(PlatformOneShotTimerTest, repeat_start_stop)
{
	TimerRunnable runnable;
	timer = OneShotTimer::create(&runnable, "TestOneShotTimer");
	CHECK(timer);
	runnable.setStartTime(100);
	timer->start(100);
	timer->start(100);

	Thread::sleep(80);

	timer->stop();
	timer->stop();

	OneShotTimer::destroy(timer);
}

TEST(PlatformOneShotTimerTest, restart)
{
	TimerRunnable runnable;
	timer = OneShotTimer::create(&runnable, "TestOneShotTimer");
	CHECK(timer);

	mock().expectOneCall("run").onObject(&runnable);
	runnable.setStartTime(100);

	timer->start(100);
	Thread::sleep(130);

	mock().expectOneCall("run").onObject(&runnable);
	runnable.setStartTime(100);

	timer->start(100);
	Thread::sleep(130);

	OneShotTimer::destroy(timer);
}

TEST(PlatformOneShotTimerTest, multiple_timers)
{
	TimerRunnable runnable1;
	OneShotTimer* timer1 = OneShotTimer::create(&runnable1, "TestOneShotTimer1");
	CHECK(timer1);

	TimerRunnable runnable2;
	OneShotTimer* timer2 = OneShotTimer::create(&runnable2, "TestOneShotTimer2");
	CHECK(timer2);

	TimerRunnable runnable3;
	OneShotTimer* timer3 = OneShotTimer::create(&runnable3, "TestOneShotTimer3");
	CHECK(timer3);

	mock().expectOneCall("run").onObject(&runnable1);
	runnable1.setStartTime(100);
	timer1->start(100);

	mock().expectOneCall("run").onObject(&runnable2);
	runnable2.setStartTime(100);
	timer2->start(100);

	Thread::sleep(50);

	mock().expectOneCall("run").onObject(&runnable3);
	runnable3.setStartTime(100);
	timer3->start(100);

	Thread::sleep(130);

	OneShotTimer::destroy(timer1);
	OneShotTimer::destroy(timer2);
	OneShotTimer::destroy(timer3);
}

TEST(PlatformOneShotTimerTest, start_stop_continuously_at_once)
{
	TimerRunnable runnable;
	timer = OneShotTimer::create(&runnable, "TestOneShotTimer");
	CHECK(timer);
	runnable.setStartTime(100);

	timer->start(100);
	timer->stop();

	runnable.setStartTime(100);

	timer->start(100);
	timer->stop();

	runnable.setStartTime(100);

	timer->start(100);
	timer->stop();

	OneShotTimer::destroy(timer);
}

TEST(PlatformOneShotTimerTest, start_destroy_at_once)
{
	TimerRunnable runnable;
	timer = OneShotTimer::create(&runnable, "TestOneShotTimer");
	CHECK(timer);
	runnable.setStartTime(100);

	timer->start(100);
	OneShotTimer::destroy(timer);
}

TEST(PlatformOneShotTimerTest, name)
{
	TimerRunnable runnable;
	timer = OneShotTimer::create(&runnable, "TestOneShotTimer");
	CHECK(timer);

	STRCMP_EQUAL("TestOneShotTimer", timer->getName());
	timer->setName("foo");
	STRCMP_EQUAL("foo", timer->getName());
	OneShotTimer::destroy(timer);
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

class MyExceptionHandler : public OneShotTimer::UncaughtExceptionHandler {
	const char* m_expectedMsg;
public:
	explicit MyExceptionHandler(const char* expectedMsg) : m_expectedMsg(expectedMsg) {}
	virtual void handle(OneShotTimer* t, const char* expectedMsg)
	{
		mock().actualCall("handle").withParameter("t", t).onObject(this);
		STRCMP_CONTAINS(m_expectedMsg, expectedMsg);
	}
};

TEST(PlatformOneShotTimerTest, exception_std)
{
	ThrowExceptionRunnable runnable(ThrowExceptionRunnable::Std);
	timer = OneShotTimer::create(&runnable, "TestOneShotTimer");

	MyExceptionHandler handler("Exception Test");
	timer->setUncaughtExceptionHandler(&handler);
	mock().expectOneCall("handle").withParameter("t", timer).onObject(&handler);

	timer->start(100);
	Thread::sleep(190);
	timer->stop();

	OneShotTimer::destroy(timer);
}

TEST(PlatformOneShotTimerTest, exception_assert)
{
	ThrowExceptionRunnable runnable(ThrowExceptionRunnable::Assert);
	timer = OneShotTimer::create(&runnable, "TestOneShotTimer");

	MyExceptionHandler handler("CHECK_ASSERT_EXCEPTION_TEST");
	timer->setUncaughtExceptionHandler(&handler);
	mock().expectOneCall("handle").withParameter("t", timer).onObject(&handler);

	timer->start(100);
	Thread::sleep(190);
	timer->stop();

	OneShotTimer::destroy(timer);
}

TEST(PlatformOneShotTimerTest, exception_unknown)
{
	ThrowExceptionRunnable runnable(ThrowExceptionRunnable::Integer);
	timer = OneShotTimer::create(&runnable, "TestOneShotTimer");

	MyExceptionHandler handler("Unknown Exception");
	timer->setUncaughtExceptionHandler(&handler);
	mock().expectOneCall("handle").withParameter("t", timer).onObject(&handler);

	timer->start(100);
	Thread::sleep(190);
	timer->stop();

	OneShotTimer::destroy(timer);
}
#endif

} // namespace PlatformOneShotTimerTest
