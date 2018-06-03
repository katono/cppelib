#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "OSWrapper/EventFlag.h"
#include "OSWrapper/EventFlagFactory.h"

using OSWrapper::EventFlag;
using OSWrapper::EventFlagFactory;
using OSWrapper::Timeout;

class TestEventFlag : public EventFlag {
	bool m_autoReset;
public:
	TestEventFlag(bool autoReset) : m_autoReset(autoReset) {}
	OSWrapper::Error waitAny()
	{
		return (OSWrapper::Error) mock().actualCall("waitAny")
			.onObject(this).returnIntValueOrDefault(OSWrapper::OK);
	}
	OSWrapper::Error waitOne(std::size_t pos)
	{
		return (OSWrapper::Error) mock()
			.actualCall("waitOne")
			.withParameter("pos", pos)
			.onObject(this).returnIntValueOrDefault(OSWrapper::OK);
	}
	OSWrapper::Error wait(Pattern bitPattern, Mode waitMode, Pattern* releasedPattern)
	{
		return (OSWrapper::Error) mock().actualCall("wait")
			.withParameter("bitPattern", bitPattern)
			.withParameter("waitMode", waitMode)
			.withOutputParameter("releasedPattern", releasedPattern)
			.onObject(this).returnIntValueOrDefault(OSWrapper::OK);
	}
	OSWrapper::Error tryWaitAny()
	{
		return (OSWrapper::Error) mock().actualCall("tryWaitAny")
			.onObject(this).returnIntValueOrDefault(OSWrapper::OK);
	}
	OSWrapper::Error tryWaitOne(std::size_t pos)
	{
		return (OSWrapper::Error) mock()
			.actualCall("tryWaitOne")
			.withParameter("pos", pos)
			.onObject(this).returnIntValueOrDefault(OSWrapper::OK);
	}
	OSWrapper::Error tryWait(Pattern bitPattern, Mode waitMode, Pattern* releasedPattern)
	{
		return (OSWrapper::Error) mock().actualCall("tryWait")
			.withParameter("bitPattern", bitPattern)
			.withParameter("waitMode", waitMode)
			.withOutputParameter("releasedPattern", releasedPattern)
			.onObject(this).returnIntValueOrDefault(OSWrapper::OK);
	}
	OSWrapper::Error timedWaitAny(Timeout tmout)
	{
		return (OSWrapper::Error) mock().actualCall("timedWaitAny")
			.withParameter("tmout", tmout)
			.onObject(this).returnIntValueOrDefault(OSWrapper::OK);
	}
	OSWrapper::Error timedWaitOne(std::size_t pos, Timeout tmout)
	{
		return (OSWrapper::Error) mock()
			.actualCall("timedWaitOne")
			.withParameter("pos", pos)
			.withParameter("tmout", tmout)
			.onObject(this).returnIntValueOrDefault(OSWrapper::OK);
	}
	OSWrapper::Error timedWait(Pattern bitPattern, Mode waitMode, Pattern* releasedPattern, Timeout tmout)
	{
		return (OSWrapper::Error) mock().actualCall("timedWait")
			.withParameter("bitPattern", bitPattern)
			.withParameter("waitMode", waitMode)
			.withOutputParameter("releasedPattern", releasedPattern)
			.withParameter("tmout", tmout)
			.onObject(this).returnIntValueOrDefault(OSWrapper::OK);
	}

	OSWrapper::Error setAll()
	{
		return (OSWrapper::Error) mock().actualCall("setAll")
			.onObject(this).returnIntValueOrDefault(OSWrapper::OK);
	}
	OSWrapper::Error setOne(std::size_t pos)
	{
		return (OSWrapper::Error) mock().actualCall("setOne")
			.withParameter("pos", pos)
			.onObject(this).returnIntValueOrDefault(OSWrapper::OK);
	}
	OSWrapper::Error set(Pattern bitPattern)
	{
		return (OSWrapper::Error) mock().actualCall("set")
			.withParameter("bitPattern", bitPattern)
			.onObject(this).returnIntValueOrDefault(OSWrapper::OK);
	}

	OSWrapper::Error resetAll()
	{
		return (OSWrapper::Error) mock().actualCall("resetAll")
			.onObject(this).returnIntValueOrDefault(OSWrapper::OK);
	}
	OSWrapper::Error resetOne(std::size_t pos)
	{
		return (OSWrapper::Error) mock().actualCall("resetOne")
			.withParameter("pos", pos)
			.onObject(this).returnIntValueOrDefault(OSWrapper::OK);
	}
	OSWrapper::Error reset(Pattern bitPattern)
	{
		return (OSWrapper::Error) mock().actualCall("reset")
			.withParameter("bitPattern", bitPattern)
			.onObject(this).returnIntValueOrDefault(OSWrapper::OK);
	}

	Pattern getCurrentPattern() const
	{
		return Pattern(mock().actualCall("getCurrentPattern")
			.onObject(this).returnIntValueOrDefault(0));
	}
};

class TestEventFlagFactory : public EventFlagFactory {
public:
	EventFlag* create(bool autoReset)
	{
		EventFlag* e = new TestEventFlag(autoReset);
		return e;
	}

	void destroy(EventFlag* e)
	{
		delete static_cast<TestEventFlag*>(e);
	}
};

TEST_GROUP(EventFlagTest) {
	TestEventFlagFactory testFactory;

	void setup()
	{
		OSWrapper::registerEventFlagFactory(&testFactory);
	}
	void teardown()
	{
		mock().checkExpectations();
		mock().clear();
	}
};

TEST(EventFlagTest, create_destroy)
{
	EventFlag* ef = EventFlag::create(true);
	CHECK(ef);
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, destroy_nullptr)
{
	EventFlag::destroy(0);
}

TEST(EventFlagTest, bit_pattern_size)
{
	EventFlag::Pattern ptn;
	LONGS_EQUAL(sizeof(unsigned int) * 8, ptn.size());
}

TEST(EventFlagTest, waitAny)
{
	EventFlag* ef = EventFlag::create(true);
	mock().expectOneCall("waitAny").onObject(ef)
		.andReturnValue(OSWrapper::OK);

	OSWrapper::Error err = ef->waitAny();
	LONGS_EQUAL(OSWrapper::OK, err);
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, waitOne)
{
	EventFlag* ef = EventFlag::create(true);
	mock().expectOneCall("waitOne").onObject(ef)
		.withParameter("pos", 0)
		.andReturnValue(OSWrapper::OK);

	OSWrapper::Error err = ef->waitOne(0);
	LONGS_EQUAL(OSWrapper::OK, err);
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, wait)
{
	EventFlag* ef = EventFlag::create(true);
	EventFlag::Pattern outputPattern(0x01);
	mock().expectOneCall("wait").onObject(ef)
		.withParameter("bitPattern", 0xFF)
		.withParameter("waitMode", EventFlag::OR)
		.withOutputParameterReturning("releasedPattern", &outputPattern, sizeof outputPattern)
		.andReturnValue(OSWrapper::OK);

	EventFlag::Pattern releasedPattern;
	OSWrapper::Error err = ef->wait(EventFlag::Pattern(0xFF), EventFlag::OR, &releasedPattern);
	LONGS_EQUAL(OSWrapper::OK, err);
	LONGS_EQUAL(0x01, releasedPattern);
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, tryWaitAny)
{
	EventFlag* ef = EventFlag::create(true);
	mock().expectOneCall("tryWaitAny").onObject(ef)
		.andReturnValue(OSWrapper::OK);

	OSWrapper::Error err = ef->tryWaitAny();
	LONGS_EQUAL(OSWrapper::OK, err);
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, tryWaitOne)
{
	EventFlag* ef = EventFlag::create(true);
	mock().expectOneCall("tryWaitOne").onObject(ef)
		.withParameter("pos", 0)
		.andReturnValue(OSWrapper::OK);

	OSWrapper::Error err = ef->tryWaitOne(0);
	LONGS_EQUAL(OSWrapper::OK, err);
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, tryWait)
{
	EventFlag* ef = EventFlag::create(true);
	EventFlag::Pattern outputPattern(0x01);
	mock().expectOneCall("tryWait").onObject(ef)
		.withParameter("bitPattern", 0xFF)
		.withParameter("waitMode", EventFlag::OR)
		.withOutputParameterReturning("releasedPattern", &outputPattern, sizeof outputPattern)
		.andReturnValue(OSWrapper::OK);

	EventFlag::Pattern releasedPattern;
	OSWrapper::Error err = ef->tryWait(EventFlag::Pattern(0xFF), EventFlag::OR, &releasedPattern);
	LONGS_EQUAL(OSWrapper::OK, err);
	LONGS_EQUAL(0x01, releasedPattern);
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, timedWaitAny_forever)
{
	EventFlag* ef = EventFlag::create(true);
	mock().expectOneCall("timedWaitAny").onObject(ef)
		.withParameter("tmout", Timeout::FOREVER)
		.andReturnValue(OSWrapper::OK);

	OSWrapper::Error err = ef->timedWaitAny(Timeout::FOREVER);
	LONGS_EQUAL(OSWrapper::OK, err);
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, timedWaitAny_polling)
{
	EventFlag* ef = EventFlag::create(true);
	mock().expectOneCall("timedWaitAny").onObject(ef)
		.withParameter("tmout", Timeout::POLLING)
		.andReturnValue(OSWrapper::TimedOut);

	OSWrapper::Error err = ef->timedWaitAny(Timeout::POLLING);
	LONGS_EQUAL(OSWrapper::TimedOut, err);
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, timedWaitAny_timeout)
{
	EventFlag* ef = EventFlag::create(true);
	mock().expectOneCall("timedWaitAny").onObject(ef)
		.withParameter("tmout", Timeout(100))
		.andReturnValue(OSWrapper::TimedOut);

	OSWrapper::Error err = ef->timedWaitAny(Timeout(100));
	LONGS_EQUAL(OSWrapper::TimedOut, err);
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, timedWaitOne_forever)
{
	EventFlag* ef = EventFlag::create(true);
	mock().expectOneCall("timedWaitOne").onObject(ef)
		.withParameter("pos", 0)
		.withParameter("tmout", Timeout::FOREVER)
		.andReturnValue(OSWrapper::OK);

	OSWrapper::Error err = ef->timedWaitOne(0, Timeout::FOREVER);
	LONGS_EQUAL(OSWrapper::OK, err);
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, timedWaitOne_polling)
{
	EventFlag* ef = EventFlag::create(true);
	mock().expectOneCall("timedWaitOne").onObject(ef)
		.withParameter("pos", 0)
		.withParameter("tmout", Timeout::POLLING)
		.andReturnValue(OSWrapper::TimedOut);

	OSWrapper::Error err = ef->timedWaitOne(0, Timeout::POLLING);
	LONGS_EQUAL(OSWrapper::TimedOut, err);
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, timedWaitOne_timeout)
{
	EventFlag* ef = EventFlag::create(true);
	mock().expectOneCall("timedWaitOne").onObject(ef)
		.withParameter("pos", 0)
		.withParameter("tmout", Timeout(100))
		.andReturnValue(OSWrapper::TimedOut);

	OSWrapper::Error err = ef->timedWaitOne(0, Timeout(100));
	LONGS_EQUAL(OSWrapper::TimedOut, err);
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, timedWait_forever)
{
	EventFlag* ef = EventFlag::create(true);
	EventFlag::Pattern outputPattern(0x01);
	mock().expectOneCall("timedWait").onObject(ef)
		.withParameter("bitPattern", 0xFF)
		.withParameter("waitMode", EventFlag::OR)
		.withOutputParameterReturning("releasedPattern", &outputPattern, sizeof outputPattern)
		.withParameter("tmout", Timeout::FOREVER)
		.andReturnValue(OSWrapper::OK);

	EventFlag::Pattern releasedPattern;
	OSWrapper::Error err = ef->timedWait(EventFlag::Pattern(0xFF), EventFlag::OR, &releasedPattern, Timeout::FOREVER);
	LONGS_EQUAL(OSWrapper::OK, err);
	LONGS_EQUAL(0x01, releasedPattern);
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, timedWait_polling)
{
	EventFlag* ef = EventFlag::create(true);
	EventFlag::Pattern outputPattern(0x01);
	mock().expectOneCall("timedWait").onObject(ef)
		.withParameter("bitPattern", 0xFF)
		.withParameter("waitMode", EventFlag::OR)
		.withOutputParameterReturning("releasedPattern", &outputPattern, sizeof outputPattern)
		.withParameter("tmout", Timeout::POLLING)
		.andReturnValue(OSWrapper::OK);

	EventFlag::Pattern releasedPattern;
	OSWrapper::Error err = ef->timedWait(EventFlag::Pattern(0xFF), EventFlag::OR, &releasedPattern, Timeout::POLLING);
	LONGS_EQUAL(OSWrapper::OK, err);
	LONGS_EQUAL(0x01, releasedPattern);
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, timedWait_timeout)
{
	EventFlag* ef = EventFlag::create(true);
	EventFlag::Pattern outputPattern(0x01);
	mock().expectOneCall("timedWait").onObject(ef)
		.withParameter("bitPattern", 0xFF)
		.withParameter("waitMode", EventFlag::OR)
		.withOutputParameterReturning("releasedPattern", &outputPattern, sizeof outputPattern)
		.withParameter("tmout", Timeout(100))
		.andReturnValue(OSWrapper::OK);

	EventFlag::Pattern releasedPattern;
	OSWrapper::Error err = ef->timedWait(EventFlag::Pattern(0xFF), EventFlag::OR, &releasedPattern, Timeout(100));
	LONGS_EQUAL(OSWrapper::OK, err);
	LONGS_EQUAL(0x01, releasedPattern);
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, setAll)
{
	EventFlag* ef = EventFlag::create(true);
	mock().expectOneCall("setAll").onObject(ef)
		.andReturnValue(OSWrapper::OK);

	OSWrapper::Error err = ef->setAll();
	LONGS_EQUAL(OSWrapper::OK, err);
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, setOne)
{
	EventFlag* ef = EventFlag::create(true);
	mock().expectOneCall("setOne").onObject(ef)
		.withParameter("pos", 0)
		.andReturnValue(OSWrapper::OK);

	OSWrapper::Error err = ef->setOne(0);
	LONGS_EQUAL(OSWrapper::OK, err);
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, set)
{
	EventFlag* ef = EventFlag::create(true);
	mock().expectOneCall("set").onObject(ef)
		.withParameter("bitPattern", 0x01)
		.andReturnValue(OSWrapper::OK);

	OSWrapper::Error err = ef->set(EventFlag::Pattern(0x01));
	LONGS_EQUAL(OSWrapper::OK, err);
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, resetAll)
{
	EventFlag* ef = EventFlag::create(true);
	mock().expectOneCall("resetAll").onObject(ef)
		.andReturnValue(OSWrapper::OK);

	OSWrapper::Error err = ef->resetAll();
	LONGS_EQUAL(OSWrapper::OK, err);
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, resetOne)
{
	EventFlag* ef = EventFlag::create(true);
	mock().expectOneCall("resetOne").onObject(ef)
		.withParameter("pos", 0)
		.andReturnValue(OSWrapper::OK);

	OSWrapper::Error err = ef->resetOne(0);
	LONGS_EQUAL(OSWrapper::OK, err);
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, reset)
{
	EventFlag* ef = EventFlag::create(true);
	mock().expectOneCall("reset").onObject(ef)
		.withParameter("bitPattern", 0x01)
		.andReturnValue(OSWrapper::OK);

	OSWrapper::Error err = ef->reset(EventFlag::Pattern(0x01));
	LONGS_EQUAL(OSWrapper::OK, err);
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, getCurrentPattern)
{
	EventFlag* ef = EventFlag::create(true);
	mock().expectOneCall("getCurrentPattern").onObject(ef)
		.andReturnValue(0x01);

	EventFlag::Pattern ptn = ef->getCurrentPattern();
	LONGS_EQUAL(0x01, ptn);
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, autoReset_true_OR)
{
	EventFlag* ef = EventFlag::create(true);
	mock().expectOneCall("set").onObject(ef)
		.withParameter("bitPattern", 0x01)
		.andReturnValue(OSWrapper::OK);
	EventFlag::Pattern outputPattern(0x01);
	mock().expectOneCall("wait").onObject(ef)
		.withParameter("bitPattern", 0xFF)
		.withParameter("waitMode", EventFlag::OR)
		.withOutputParameterReturning("releasedPattern", &outputPattern, sizeof outputPattern)
		.andReturnValue(OSWrapper::OK);
	mock().expectOneCall("getCurrentPattern").onObject(ef)
		.andReturnValue(0x00);

	OSWrapper::Error err = ef->set(EventFlag::Pattern(0x01));
	LONGS_EQUAL(OSWrapper::OK, err);

	{
		// This block is an intention like other thread execution
		EventFlag::Pattern releasedPattern;
		err = ef->wait(EventFlag::Pattern(0xFF), EventFlag::OR, &releasedPattern);
		LONGS_EQUAL(OSWrapper::OK, err);
		LONGS_EQUAL(0x01, releasedPattern);

		EventFlag::Pattern ptn = ef->getCurrentPattern();
		LONGS_EQUAL(0x00, ptn);
	}

	EventFlag::destroy(ef);
}

TEST(EventFlagTest, autoReset_false_AND)
{
	EventFlag* ef = EventFlag::create(false);
	mock().expectOneCall("set").onObject(ef)
		.withParameter("bitPattern", 0x01)
		.andReturnValue(OSWrapper::OK);
	mock().expectOneCall("set").onObject(ef)
		.withParameter("bitPattern", 0x02)
		.andReturnValue(OSWrapper::OK);
	EventFlag::Pattern outputPattern(0x03);
	mock().expectOneCall("wait").onObject(ef)
		.withParameter("bitPattern", 0x03)
		.withParameter("waitMode", EventFlag::AND)
		.withOutputParameterReturning("releasedPattern", &outputPattern, sizeof outputPattern)
		.andReturnValue(OSWrapper::OK);
	mock().expectOneCall("getCurrentPattern").onObject(ef)
		.andReturnValue(0x03);
	mock().expectOneCall("resetAll").onObject(ef)
		.andReturnValue(OSWrapper::OK);
	mock().expectOneCall("getCurrentPattern").onObject(ef)
		.andReturnValue(0x00);

	OSWrapper::Error err = ef->set(EventFlag::Pattern(0x01));
	LONGS_EQUAL(OSWrapper::OK, err);
	err = ef->set(EventFlag::Pattern(0x02));
	LONGS_EQUAL(OSWrapper::OK, err);

	{
		// This block is an intention like other thread execution
		EventFlag::Pattern releasedPattern;
		err = ef->wait(EventFlag::Pattern(0x03), EventFlag::AND, &releasedPattern);
		LONGS_EQUAL(OSWrapper::OK, err);
		LONGS_EQUAL(0x03, releasedPattern);

		EventFlag::Pattern ptn = ef->getCurrentPattern();
		LONGS_EQUAL(0x03, ptn);
		err = ef->resetAll();
		LONGS_EQUAL(OSWrapper::OK, err);
		ptn = ef->getCurrentPattern();
		LONGS_EQUAL(0x00, ptn);
	}

	EventFlag::destroy(ef);
}

