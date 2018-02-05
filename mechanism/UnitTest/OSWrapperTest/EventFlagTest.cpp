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
	EventFlag::Error waitAny(Timeout tmout)
	{
		return (EventFlag::Error) mock().actualCall("waitAny")
			.withParameter("tmout", tmout)
			.onObject(this).returnIntValueOrDefault(EventFlag::OK);
	}
	EventFlag::Error waitOne(std::size_t pos, Timeout tmout)
	{
		return (EventFlag::Error) mock()
			.actualCall("waitOne")
			.withParameter("pos", pos)
			.withParameter("tmout", tmout)
			.onObject(this).returnIntValueOrDefault(EventFlag::OK);
	}
	EventFlag::Error wait(Pattern bitPattern, Mode waitMode, Pattern* releasedPattern, Timeout tmout)
	{
		return (EventFlag::Error) mock().actualCall("wait")
			.withParameter("bitPattern", bitPattern.data())
			.withParameter("waitMode", waitMode)
			.withOutputParameter("releasedPattern", releasedPattern)
			.withParameter("tmout", tmout)
			.onObject(this).returnIntValueOrDefault(EventFlag::OK);
	}

	EventFlag::Error setAll()
	{
		return (EventFlag::Error) mock().actualCall("setAll")
			.onObject(this).returnIntValueOrDefault(EventFlag::OK);
	}
	EventFlag::Error setOne(std::size_t pos)
	{
		return (EventFlag::Error) mock().actualCall("setOne")
			.withParameter("pos", pos)
			.onObject(this).returnIntValueOrDefault(EventFlag::OK);
	}
	EventFlag::Error set(Pattern bitPattern)
	{
		return (EventFlag::Error) mock().actualCall("set")
			.withParameter("bitPattern", bitPattern.data())
			.onObject(this).returnIntValueOrDefault(EventFlag::OK);
	}

	EventFlag::Error resetAll()
	{
		return (EventFlag::Error) mock().actualCall("resetAll")
			.onObject(this).returnIntValueOrDefault(EventFlag::OK);
	}
	EventFlag::Error resetOne(std::size_t pos)
	{
		return (EventFlag::Error) mock().actualCall("resetOne")
			.withParameter("pos", pos)
			.onObject(this).returnIntValueOrDefault(EventFlag::OK);
	}
	EventFlag::Error reset(Pattern bitPattern)
	{
		return (EventFlag::Error) mock().actualCall("reset")
			.withParameter("bitPattern", bitPattern.data())
			.onObject(this).returnIntValueOrDefault(EventFlag::OK);
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
		delete e;
	}
};

TEST_GROUP(EventFlagTest) {
	TestEventFlagFactory testFactory;

	void setup()
	{
		EventFlag::setFactory(&testFactory);
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
		.withParameter("tmout", Timeout::FOREVER)
		.andReturnValue(EventFlag::OK);

	EventFlag::Error err = ef->waitAny();
	LONGS_EQUAL(EventFlag::OK, err);
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, waitAny_polling)
{
	EventFlag* ef = EventFlag::create(true);
	mock().expectOneCall("waitAny").onObject(ef)
		.withParameter("tmout", Timeout::POLLING)
		.andReturnValue(EventFlag::TimedOut);

	EventFlag::Error err = ef->waitAny(Timeout::POLLING);
	LONGS_EQUAL(EventFlag::TimedOut, err);
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, waitAny_timeout)
{
	EventFlag* ef = EventFlag::create(true);
	mock().expectOneCall("waitAny").onObject(ef)
		.withParameter("tmout", Timeout(100))
		.andReturnValue(EventFlag::TimedOut);

	EventFlag::Error err = ef->waitAny(Timeout(100));
	LONGS_EQUAL(EventFlag::TimedOut, err);
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, waitOne)
{
	EventFlag* ef = EventFlag::create(true);
	mock().expectOneCall("waitOne").onObject(ef)
		.withParameter("pos", 0)
		.withParameter("tmout", Timeout::FOREVER)
		.andReturnValue(EventFlag::OK);

	EventFlag::Error err = ef->waitOne(0);
	LONGS_EQUAL(EventFlag::OK, err);
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, waitOne_polling)
{
	EventFlag* ef = EventFlag::create(true);
	mock().expectOneCall("waitOne").onObject(ef)
		.withParameter("pos", 0)
		.withParameter("tmout", Timeout::POLLING)
		.andReturnValue(EventFlag::TimedOut);

	EventFlag::Error err = ef->waitOne(0, Timeout::POLLING);
	LONGS_EQUAL(EventFlag::TimedOut, err);
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, waitOne_timeout)
{
	EventFlag* ef = EventFlag::create(true);
	mock().expectOneCall("waitOne").onObject(ef)
		.withParameter("pos", 0)
		.withParameter("tmout", Timeout(100))
		.andReturnValue(EventFlag::TimedOut);

	EventFlag::Error err = ef->waitOne(0, Timeout(100));
	LONGS_EQUAL(EventFlag::TimedOut, err);
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
		.withParameter("tmout", Timeout::FOREVER)
		.andReturnValue(EventFlag::OK);

	EventFlag::Pattern releasedPattern;
	EventFlag::Error err = ef->wait(EventFlag::Pattern(0xFF), EventFlag::OR, &releasedPattern);
	LONGS_EQUAL(EventFlag::OK, err);
	LONGS_EQUAL(0x01, releasedPattern.data());
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, wait_polling)
{
	EventFlag* ef = EventFlag::create(true);
	EventFlag::Pattern outputPattern(0x01);
	mock().expectOneCall("wait").onObject(ef)
		.withParameter("bitPattern", 0xFF)
		.withParameter("waitMode", EventFlag::OR)
		.withOutputParameterReturning("releasedPattern", &outputPattern, sizeof outputPattern)
		.withParameter("tmout", Timeout::POLLING)
		.andReturnValue(EventFlag::OK);

	EventFlag::Pattern releasedPattern;
	EventFlag::Error err = ef->wait(EventFlag::Pattern(0xFF), EventFlag::OR, &releasedPattern, Timeout::POLLING);
	LONGS_EQUAL(EventFlag::OK, err);
	LONGS_EQUAL(0x01, releasedPattern.data());
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, wait_timeout)
{
	EventFlag* ef = EventFlag::create(true);
	EventFlag::Pattern outputPattern(0x01);
	mock().expectOneCall("wait").onObject(ef)
		.withParameter("bitPattern", 0xFF)
		.withParameter("waitMode", EventFlag::OR)
		.withOutputParameterReturning("releasedPattern", &outputPattern, sizeof outputPattern)
		.withParameter("tmout", Timeout(100))
		.andReturnValue(EventFlag::OK);

	EventFlag::Pattern releasedPattern;
	EventFlag::Error err = ef->wait(EventFlag::Pattern(0xFF), EventFlag::OR, &releasedPattern, Timeout(100));
	LONGS_EQUAL(EventFlag::OK, err);
	LONGS_EQUAL(0x01, releasedPattern.data());
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, setAll)
{
	EventFlag* ef = EventFlag::create(true);
	mock().expectOneCall("setAll").onObject(ef)
		.andReturnValue(EventFlag::OK);

	EventFlag::Error err = ef->setAll();
	LONGS_EQUAL(EventFlag::OK, err);
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, setOne)
{
	EventFlag* ef = EventFlag::create(true);
	mock().expectOneCall("setOne").onObject(ef)
		.withParameter("pos", 0)
		.andReturnValue(EventFlag::OK);

	EventFlag::Error err = ef->setOne(0);
	LONGS_EQUAL(EventFlag::OK, err);
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, set)
{
	EventFlag* ef = EventFlag::create(true);
	mock().expectOneCall("set").onObject(ef)
		.withParameter("bitPattern", 0x01)
		.andReturnValue(EventFlag::OK);

	EventFlag::Error err = ef->set(EventFlag::Pattern(0x01));
	LONGS_EQUAL(EventFlag::OK, err);
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, resetAll)
{
	EventFlag* ef = EventFlag::create(true);
	mock().expectOneCall("resetAll").onObject(ef)
		.andReturnValue(EventFlag::OK);

	EventFlag::Error err = ef->resetAll();
	LONGS_EQUAL(EventFlag::OK, err);
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, resetOne)
{
	EventFlag* ef = EventFlag::create(true);
	mock().expectOneCall("resetOne").onObject(ef)
		.withParameter("pos", 0)
		.andReturnValue(EventFlag::OK);

	EventFlag::Error err = ef->resetOne(0);
	LONGS_EQUAL(EventFlag::OK, err);
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, reset)
{
	EventFlag* ef = EventFlag::create(true);
	mock().expectOneCall("reset").onObject(ef)
		.withParameter("bitPattern", 0x01)
		.andReturnValue(EventFlag::OK);

	EventFlag::Error err = ef->reset(EventFlag::Pattern(0x01));
	LONGS_EQUAL(EventFlag::OK, err);
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, getCurrentPattern)
{
	EventFlag* ef = EventFlag::create(true);
	mock().expectOneCall("getCurrentPattern").onObject(ef)
		.andReturnValue(0x01);

	EventFlag::Pattern ptn = ef->getCurrentPattern();
	LONGS_EQUAL(0x01, ptn.data());
	EventFlag::destroy(ef);
}

TEST(EventFlagTest, autoReset_true_OR)
{
	EventFlag* ef = EventFlag::create(true);
	mock().expectOneCall("set").onObject(ef)
		.withParameter("bitPattern", 0x01)
		.andReturnValue(EventFlag::OK);
	EventFlag::Pattern outputPattern(0x01);
	mock().expectOneCall("wait").onObject(ef)
		.withParameter("bitPattern", 0xFF)
		.withParameter("waitMode", EventFlag::OR)
		.withOutputParameterReturning("releasedPattern", &outputPattern, sizeof outputPattern)
		.withParameter("tmout", Timeout::FOREVER)
		.andReturnValue(EventFlag::OK);
	mock().expectOneCall("getCurrentPattern").onObject(ef)
		.andReturnValue(0x00);

	EventFlag::Error err = ef->set(EventFlag::Pattern(0x01));
	LONGS_EQUAL(EventFlag::OK, err);

	{
		// This block is an intention like other thread execution
		EventFlag::Pattern releasedPattern;
		err = ef->wait(EventFlag::Pattern(0xFF), EventFlag::OR, &releasedPattern);
		LONGS_EQUAL(EventFlag::OK, err);
		LONGS_EQUAL(0x01, releasedPattern.data());

		EventFlag::Pattern ptn = ef->getCurrentPattern();
		LONGS_EQUAL(0x00, ptn.data());
	}

	EventFlag::destroy(ef);
}

TEST(EventFlagTest, autoReset_false_AND)
{
	EventFlag* ef = EventFlag::create(false);
	mock().expectOneCall("set").onObject(ef)
		.withParameter("bitPattern", 0x01)
		.andReturnValue(EventFlag::OK);
	mock().expectOneCall("set").onObject(ef)
		.withParameter("bitPattern", 0x02)
		.andReturnValue(EventFlag::OK);
	EventFlag::Pattern outputPattern(0x03);
	mock().expectOneCall("wait").onObject(ef)
		.withParameter("bitPattern", 0x03)
		.withParameter("waitMode", EventFlag::AND)
		.withOutputParameterReturning("releasedPattern", &outputPattern, sizeof outputPattern)
		.withParameter("tmout", Timeout::FOREVER)
		.andReturnValue(EventFlag::OK);
	mock().expectOneCall("getCurrentPattern").onObject(ef)
		.andReturnValue(0x03);
	mock().expectOneCall("resetAll").onObject(ef)
		.andReturnValue(EventFlag::OK);
	mock().expectOneCall("getCurrentPattern").onObject(ef)
		.andReturnValue(0x00);

	EventFlag::Error err = ef->set(EventFlag::Pattern(0x01));
	LONGS_EQUAL(EventFlag::OK, err);
	err = ef->set(EventFlag::Pattern(0x02));
	LONGS_EQUAL(EventFlag::OK, err);

	{
		// This block is an intention like other thread execution
		EventFlag::Pattern releasedPattern;
		err = ef->wait(EventFlag::Pattern(0x03), EventFlag::AND, &releasedPattern);
		LONGS_EQUAL(EventFlag::OK, err);
		LONGS_EQUAL(0x03, releasedPattern.data());

		EventFlag::Pattern ptn = ef->getCurrentPattern();
		LONGS_EQUAL(0x03, ptn.data());
		err = ef->resetAll();
		LONGS_EQUAL(EventFlag::OK, err);
		ptn = ef->getCurrentPattern();
		LONGS_EQUAL(0x00, ptn.data());
	}

	EventFlag::destroy(ef);
}

