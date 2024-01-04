#ifdef CPPELIB_NO_EXCEPTIONS

#include "Assertion/Assertion.h"
#include <csetjmp>
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

namespace {
std::jmp_buf s_jmpBuf;

class TestAssert : public Assertion::AssertHandler {
public:
	void handle(const char* msg)
	{
		STRCMP_CONTAINS(__FILE__, msg);
		STRCMP_CONTAINS("Assertion failed", msg);
		mock().actualCall("handle").onObject(this);
		std::longjmp(s_jmpBuf, -1); // pseudo abort
	}
};

}

TEST_GROUP(AssertionNoExceptionsTest) {
	void setup()
	{
	}
	void teardown()
	{
		mock().checkExpectations();
		mock().clear();

		Assertion::setHandler(0); // reset handler
	}
};

TEST(AssertionNoExceptionsTest, assert_true)
{
	static TestAssert testAssert;
	Assertion::setHandler(&testAssert);

	if (setjmp(s_jmpBuf) == 0) {
		bool a = true;
		CHECK_ASSERT(a);
	} else {
		FAIL("failed");
	}
}

TEST(AssertionNoExceptionsTest, assert_false)
{
	static TestAssert testAssert;
	Assertion::setHandler(&testAssert);
	mock().expectOneCall("handle").onObject(&testAssert);

	volatile int ret;
	if ((ret = setjmp(s_jmpBuf)) == 0) {
		bool a = false;
		CHECK_ASSERT(a);
		FAIL("failed");
	} else {
		LONGS_EQUAL(-1, ret);
		return;
	}
	FAIL("failed");
}
#endif
