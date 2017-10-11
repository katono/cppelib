#include "CppUTest/TestHarness.h"
#include "DesignByContract/Assertion.h"
#include <stdio.h>
#include <string>


TEST_GROUP(AssertionTest) {
	class Test {
		std::string m_str;
	public:
		void foo(const char* str)
		{
			DBC_PRE(str);
			m_str = str;
		}
		void bar()
		{
			DBC_POST(m_str.length() > 0);
		}
	};
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
		DBC_ASSERT(a);
	}
	catch (...) {
		FAIL("failed");
	}
}

TEST(AssertionTest, assert_false)
{
	try {
		bool a = false;
		DBC_ASSERT(a);
	}
	catch (const std::exception& e) {
		STRCMP_CONTAINS("Assertion failed", e.what());
		return;
	}
	FAIL("failed");
}

TEST(AssertionTest, precondition_OK)
{
	try {
		Test t;
		t.foo("foo");
	}
	catch (...) {
		FAIL("failed");
	}
}

TEST(AssertionTest, precondition_failed)
{
	try {
		Test t;
		t.foo(0);
	}
	catch (const std::exception& e) {
		STRCMP_CONTAINS("Pre-condition failed", e.what());
		return;
	}
	FAIL("failed");
}

TEST(AssertionTest, postcondition_OK)
{
	try {
		Test t;
		t.foo("foo");
		t.bar();
	}
	catch (...) {
		FAIL("failed");
	}
}

TEST(AssertionTest, postcondition_failed)
{
	try {
		Test t;
		t.bar();
	}
	catch (const std::exception& e) {
		STRCMP_CONTAINS("Post-condition failed", e.what());
		return;
	}
	FAIL("failed");
}

