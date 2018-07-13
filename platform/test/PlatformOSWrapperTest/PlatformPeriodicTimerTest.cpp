#include "OSWrapper/Runnable.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/Mutex.h"
#include "OSWrapper/EventFlag.h"
#include "OSWrapper/PeriodicTimer.h"
#include "Assertion/Assertion.h"
#include <stdexcept>

#ifdef PLATFORM_OS_WINDOWS
#include <chrono>
#include <windows.h>
#include "WindowsOSWrapper/WindowsThreadFactory.h"
#include "WindowsOSWrapper/WindowsMutexFactory.h"
#include "WindowsOSWrapper/WindowsEventFlagFactory.h"
#include "WindowsOSWrapper/WindowsPeriodicTimerFactory.h"
typedef WindowsOSWrapper::WindowsThreadFactory PlatformThreadFactory;
typedef WindowsOSWrapper::WindowsMutexFactory PlatformMutexFactory;
typedef WindowsOSWrapper::WindowsEventFlagFactory PlatformEventFlagFactory;
typedef WindowsOSWrapper::WindowsPeriodicTimerFactory PlatformPeriodicTimerFactory;
#elif PLATFORM_OS_ITRON
#include "ItronOSWrapper/ItronThreadFactory.h"
#include "ItronOSWrapper/ItronMutexFactory.h"
#include "ItronOSWrapper/ItronEventFlagFactory.h"
#include "ItronOSWrapper/ItronPeriodicTimerFactory.h"
typedef ItronOSWrapper::ItronThreadFactory PlatformThreadFactory;
typedef ItronOSWrapper::ItronMutexFactory PlatformMutexFactory;
typedef ItronOSWrapper::ItronEventFlagFactory PlatformEventFlagFactory;
typedef ItronOSWrapper::ItronPeriodicTimerFactory PlatformPeriodicTimerFactory;
#endif

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
	PlatformThreadFactory testThreadFactory;
	PlatformMutexFactory testMutexFactory;
	PlatformEventFlagFactory testEventFlagFactory;
	PlatformPeriodicTimerFactory testPeriodicTimerFactory;

	PeriodicTimer* timer;

	void setup()
	{
		OSWrapper::registerMutexFactory(&testMutexFactory);
		OSWrapper::registerThreadFactory(&testThreadFactory);
		OSWrapper::registerEventFlagFactory(&testEventFlagFactory);
		OSWrapper::registerPeriodicTimerFactory(&testPeriodicTimerFactory);

		s_mutex = Mutex::create();
	}
	void teardown()
	{
		Mutex::destroy(s_mutex);

		mock().checkExpectations();
		mock().clear();
	}
};

class TimerRunnable : public Runnable {
	unsigned long m_period;
	unsigned long m_prevTime;
	unsigned long getTime()
	{
#ifdef PLATFORM_OS_WINDOWS
		auto now = std::chrono::system_clock::now();
		auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
		return static_cast<unsigned long>(now_ms.count());

#elif PLATFORM_OS_ITRON
		SYSTIM tim;
		get_tim(&tim);
		// TODO: Fix return value, because SYSTIM is implementation dependent
		return 0;
#else
		return 0;
#endif
	}
public:
	TimerRunnable(unsigned long period) : m_period(period), m_prevTime(0) {}
	unsigned long getPeriod() const { return m_period; }
	void run()
	{
		if (m_prevTime == 0) {
			m_prevTime = getTime();
			return;
		}
		unsigned long time = getTime();
		unsigned long diff = time - m_prevTime;
		if (diff < m_period - 3 || m_period + 3 < diff) {
			FAIL(StringFromFormat("period:%ld, diff:%ld", m_period, diff).asCharString());
		}
		m_prevTime = time;
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
	timer->start();

	CHECK(timer->isStarted());

	Thread::sleep(1000);

	timer->stop();
	CHECK(!timer->isStarted());

	PeriodicTimer::destroy(timer);
}

TEST(PlatformPeriodicTimerTest, repeat_start_stop)
{
	TimerRunnable runnable(100);
	timer = PeriodicTimer::create(&runnable, runnable.getPeriod(), "TestPeriodicTimer");
	CHECK(timer);
	timer->start();
	timer->start();

	Thread::sleep(200);

	timer->stop();
	timer->stop();

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


} // namespace PlatformPeriodicTimerTest
