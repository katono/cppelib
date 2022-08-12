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
	int failureLine = 0;
	try {
		try {
			bool a = false;
			failureLine = __LINE__; CHECK_ASSERT(a);
		}
		catch (const std::exception&) {
			FAIL("NOT std::exception");
		}
	}
	catch (const Assertion::Failure& e) {
		STRCMP_CONTAINS(__FILE__, e.message());
		STRCMP_CONTAINS(StringFrom(failureLine).asCharString(), e.message());
		STRCMP_CONTAINS("Assertion failed", e.message());
		return;
	}
	FAIL("failed");
}

#ifndef NDEBUG
TEST(AssertionTest, debug_assert_false)
{
	int failureLine = 0;
	try {
		try {
			bool a = false;
			failureLine = __LINE__; DEBUG_ASSERT(a);
		}
		catch (const std::exception&) {
			FAIL("NOT std::exception");
		}
	}
	catch (const Assertion::Failure& e) {
		STRCMP_CONTAINS(__FILE__, e.message());
		STRCMP_CONTAINS(StringFrom(failureLine).asCharString(), e.message());
		STRCMP_CONTAINS("Assertion failed", e.message());
		return;
	}
	FAIL("failed");
}
#endif

TEST(AssertionTest, failure_message)
{
	const struct TestData {
		SimpleString file;
		unsigned int line;
		SimpleString expr;
		SimpleString message;
	} testData[] = {
		{"hoge.cpp", 123, "hoge != nullptr", "hoge.cpp(123): Assertion failed (hoge != nullptr)"},
		{"/foo/bar/baz/hoge/piyo.cpp", 123, "hoge != nullptr", "/foo/bar/baz/hoge/piyo.cpp(123): Assertion failed (hoge != nullptr)"},
		{"", 1, "", "(1): Assertion failed ()"},
		{"hoge.cpp", 4294967295, "hoge != nullptr", "hoge.cpp(4294967295): Assertion failed (hoge != nullptr)"},
		{SimpleString("a", 500), 123, "hoge != nullptr", SimpleString("a", 500) + "(123): Asse"},
	};
	for (unsigned int i = 0; i < sizeof testData / sizeof testData[0]; i++) {
		const TestData& d = testData[i];
		Assertion::Failure f(d.file.asCharString(), d.line, d.expr.asCharString());
		STRCMP_EQUAL(d.message.asCharString(), f.message());
	}
}
