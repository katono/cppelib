#include "Assertion/Assertion.h"
#include <string>
#include "CppUTest/TestHarness.h"


TEST_GROUP(AssertionTest) {
	void setup()
	{
	}
	void teardown()
	{
	}
};

TEST(AssertionTest, assert_true)
{
	try {
		bool a = true;
		CHECK_ASSERT(a);
	}
	catch (...) {
		FAIL("failed");
	}
}

TEST(AssertionTest, assert_false)
{
	try {
		try {
			bool a = false;
			CHECK_ASSERT(a);
		}
		catch (const std::exception&) {
			FAIL("NOT std::exception");
		}
	}
	catch (const Assertion::Failure& e) {
		STRCMP_CONTAINS("Assertion failed", e.message());
		return;
	}
	FAIL("failed");
}

#ifndef NDEBUG
TEST(AssertionTest, debug_assert_false)
{
	try {
		try {
			bool a = false;
			DEBUG_ASSERT(a);
		}
		catch (const std::exception&) {
			FAIL("NOT std::exception");
		}
	}
	catch (const Assertion::Failure& e) {
		STRCMP_CONTAINS("Assertion failed", e.message());
		return;
	}
	FAIL("failed");
}
#endif

