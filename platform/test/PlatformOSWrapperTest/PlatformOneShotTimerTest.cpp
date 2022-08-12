#include "OSWrapper/Runnable.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/Mutex.h"
#include "OSWrapper/EventFlag.h"
#include "OSWrapper/OneShotTimer.h"
#include "Assertion/Assertion.h"
#include <stdexcept>

#if defined(PLATFORM_OS_WINDOWS)
#include <chrono>
#include <windows.h>
#include "WindowsOSWrapper/WindowsThreadFactory.h"
#include "WindowsOSWrapper/WindowsMutexFactory.h"
#include "WindowsOSWrapper/WindowsEventFlagFactory.h"
#include "WindowsOSWrapper/WindowsOneShotTimerFactory.h"
typedef WindowsOSWrapper::WindowsThreadFactory PlatformThreadFactory;
typedef WindowsOSWrapper::WindowsMutexFactory PlatformMutexFactory;
typedef WindowsOSWrapper::WindowsEventFlagFactory PlatformEventFlagFactory;
typedef WindowsOSWrapper::WindowsOneShotTimerFactory PlatformOneShotTimerFactory;
#elif defined(PLATFORM_OS_POSIX)
#include <chrono>
#include <unistd.h>
#include "PosixOSWrapper/PosixThreadFactory.h"
#include "PosixOSWrapper/PosixMutexFactory.h"
#include "PosixOSWrapper/PosixEventFlagFactory.h"
#include "PosixOSWrapper/PosixOneShotTimerFactory.h"
typedef PosixOSWrapper::PosixThreadFactory PlatformThreadFactory;
typedef PosixOSWrapper::PosixMutexFactory PlatformMutexFactory;
typedef PosixOSWrapper::PosixEventFlagFactory PlatformEventFlagFactory;
typedef PosixOSWrapper::PosixOneShotTimerFactory PlatformOneShotTimerFactory;
#elif defined(PLATFORM_OS_STDCPP)
#include <chrono>
#include "StdCppOSWrapper/StdCppThreadFactory.h"
#include "StdCppOSWrapper/StdCppMutexFactory.h"
#include "StdCppOSWrapper/StdCppEventFlagFactory.h"
#include "StdCppOSWrapper/StdCppOneShotTimerFactory.h"
typedef StdCppOSWrapper::StdCppThreadFactory PlatformThreadFactory;
typedef StdCppOSWrapper::StdCppMutexFactory PlatformMutexFactory;
typedef StdCppOSWrapper::StdCppEventFlagFactory PlatformEventFlagFactory;
typedef StdCppOSWrapper::StdCppOneShotTimerFactory PlatformOneShotTimerFactory;
#elif defined(PLATFORM_OS_ITRON)
#include "ItronOSWrapper/ItronThreadFactory.h"
#include "ItronOSWrapper/ItronMutexFactory.h"
#include "ItronOSWrapper/ItronEventFlagFactory.h"
#include "ItronOSWrapper/ItronOneShotTimerFactory.h"
typedef ItronOSWrapper::ItronThreadFactory PlatformThreadFactory;
typedef ItronOSWrapper::ItronMutexFactory PlatformMutexFactory;
typedef ItronOSWrapper::ItronEventFlagFactory PlatformEventFlagFactory;
typedef ItronOSWrapper::ItronOneShotTimerFactory PlatformOneShotTimerFactory;
#endif

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
	PlatformThreadFactory testThreadFactory;
	PlatformMutexFactory testMutexFactory;
	PlatformEventFlagFactory testEventFlagFactory;
	PlatformOneShotTimerFactory testOneShotTimerFactory;

	OneShotTimer* timer;

	void setup()
	{
		OSWrapper::registerMutexFactory(&testMutexFactory);
		OSWrapper::registerThreadFactory(&testThreadFactory);
		OSWrapper::registerEventFlagFactory(&testEventFlagFactory);
		OSWrapper::registerOneShotTimerFactory(&testOneShotTimerFactory);

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
	unsigned long m_time;
	unsigned long m_prevTime;
	unsigned long getTime()
	{
#if defined(PLATFORM_OS_WINDOWS) || defined(PLATFORM_OS_POSIX) || defined(PLATFORM_OS_STDCPP)
		auto now = std::chrono::system_clock::now();
		auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
		return static_cast<unsigned long>(now_ms.count());
#elif defined(PLATFORM_OS_ITRON)
		SYSTIM tim;
		get_tim(&tim);
		// TODO: Fix return value, because SYSTIM is implementation dependent
		return 0;
#else
		return 0;
#endif
	}
public:
	TimerRunnable() : m_time(0), m_prevTime(0) {}
	void setStartTime(unsigned long time)
	{
		m_time = time;
		m_prevTime = getTime();
	}
	void run()
	{
		if (m_prevTime == 0) {
			m_prevTime = getTime();
			return;
		}
		unsigned long time = getTime();
		unsigned long diff = time - m_prevTime;
#if defined(PLATFORM_OS_WINDOWS) || defined(PLATFORM_OS_POSIX) || defined(PLATFORM_OS_STDCPP)
		unsigned long tolerance = 20;
#else
		unsigned long tolerance = 3;
#endif
		if (diff < m_time - tolerance || m_time + tolerance < diff) {
			LockGuard lock(s_mutex);
			FAIL(StringFromFormat("time:%ld, diff:%ld", m_time, diff).asCharString());
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
