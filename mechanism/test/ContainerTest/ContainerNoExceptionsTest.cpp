#ifdef CPPELIB_NO_EXCEPTIONS

#include "Assertion/Assertion.h"
#include "Container/Array.h"
#include "Container/FixedVector.h"
#include "Container/FixedDeque.h"
#include "Container/PreallocatedVector.h"
#include "Container/PreallocatedDeque.h"
#include <string>
#include <csetjmp>
#include <cstdio>
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

using Container::Array;
using Container::FixedVector;
using Container::FixedDeque;
using Container::PreallocatedVector;
using Container::PreallocatedDeque;

namespace {
std::jmp_buf s_jmpBuf;

class TestAssert : public Assertion::AssertHandler {
public:
	std::string m_msg;
	void handle(const char* msg)
	{
		m_msg = msg;
		mock().actualCall("handle").onObject(this);
		std::longjmp(s_jmpBuf, -1);
	}
};

}

TEST_GROUP(ContainerNoExceptionsTest) {
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

TEST(ContainerNoExceptionsTest, Array_test)
{
	TestAssert testAssert;
	Assertion::setHandler(&testAssert);

	Array<int, 10> x;
	if (setjmp(s_jmpBuf) == 0) {
		x.at(9);
	} else {
		FAIL("failed");
	}

	mock().expectOneCall("handle").onObject(&testAssert);
	if (setjmp(s_jmpBuf) == 0) {
		x.at(10);
		FAIL("failed");
	} else {
		STRCMP_CONTAINS("OutOfRange", testAssert.m_msg.c_str());
		STRCMP_CONTAINS("Array::at", testAssert.m_msg.c_str());
		return;
	}
	FAIL("failed");
}

TEST(ContainerNoExceptionsTest, FixedVector_test)
{
	TestAssert testAssert;
	Assertion::setHandler(&testAssert);

	FixedVector<int, 10> x;
	if (setjmp(s_jmpBuf) == 0) {
		x.resize(10);
		x.at(9);
	} else {
		FAIL("failed");
	}

	mock().expectOneCall("handle").onObject(&testAssert);
	if (setjmp(s_jmpBuf) == 0) {
		x.at(10);
		FAIL("failed");
	} else {
		STRCMP_CONTAINS("OutOfRange", testAssert.m_msg.c_str());
		STRCMP_CONTAINS("FixedVector::at", testAssert.m_msg.c_str());
	}

	mock().expectOneCall("handle").onObject(&testAssert);
	if (setjmp(s_jmpBuf) == 0) {
		x.push_back(123);
		FAIL("failed");
	} else {
		STRCMP_CONTAINS("BadAlloc", testAssert.m_msg.c_str());
		return;
	}
	FAIL("failed");
}

TEST(ContainerNoExceptionsTest, FixedDeque_test)
{
	TestAssert testAssert;
	Assertion::setHandler(&testAssert);

	FixedDeque<int, 10> x;
	if (setjmp(s_jmpBuf) == 0) {
		x.resize(10);
		x.at(9);
	} else {
		FAIL("failed");
	}

	mock().expectOneCall("handle").onObject(&testAssert);
	if (setjmp(s_jmpBuf) == 0) {
		x.at(10);
		FAIL("failed");
	} else {
		STRCMP_CONTAINS("OutOfRange", testAssert.m_msg.c_str());
		STRCMP_CONTAINS("FixedDeque::at", testAssert.m_msg.c_str());
	}

	mock().expectOneCall("handle").onObject(&testAssert);
	if (setjmp(s_jmpBuf) == 0) {
		x.push_back(123);
		FAIL("failed");
	} else {
		STRCMP_CONTAINS("BadAlloc", testAssert.m_msg.c_str());
		return;
	}
	FAIL("failed");
}

TEST(ContainerNoExceptionsTest, PreallocatedVector_test)
{
	TestAssert testAssert;
	Assertion::setHandler(&testAssert);

	int buf[10];
	PreallocatedVector<int> x(buf, sizeof buf);
	if (setjmp(s_jmpBuf) == 0) {
		x.resize(10);
		x.at(9);
	} else {
		FAIL("failed");
	}

	mock().expectOneCall("handle").onObject(&testAssert);
	if (setjmp(s_jmpBuf) == 0) {
		x.at(10);
		FAIL("failed");
	} else {
		STRCMP_CONTAINS("OutOfRange", testAssert.m_msg.c_str());
		STRCMP_CONTAINS("PreallocatedVector::at", testAssert.m_msg.c_str());
	}

	mock().expectOneCall("handle").onObject(&testAssert);
	if (setjmp(s_jmpBuf) == 0) {
		x.push_back(123);
		FAIL("failed");
	} else {
		STRCMP_CONTAINS("BadAlloc", testAssert.m_msg.c_str());
		return;
	}
	FAIL("failed");
}

TEST(ContainerNoExceptionsTest, PreallocatedDeque_test)
{
	TestAssert testAssert;
	Assertion::setHandler(&testAssert);

	int buf[11];
	PreallocatedDeque<int> x(buf, sizeof buf);
	if (setjmp(s_jmpBuf) == 0) {
		x.resize(10);
		x.at(9);
	} else {
		FAIL("failed");
	}

	mock().expectOneCall("handle").onObject(&testAssert);
	if (setjmp(s_jmpBuf) == 0) {
		x.at(10);
		FAIL("failed");
	} else {
		STRCMP_CONTAINS("OutOfRange", testAssert.m_msg.c_str());
		STRCMP_CONTAINS("PreallocatedDeque::at", testAssert.m_msg.c_str());
	}

	mock().expectOneCall("handle").onObject(&testAssert);
	if (setjmp(s_jmpBuf) == 0) {
		x.push_back(123);
		FAIL("failed");
	} else {
		STRCMP_CONTAINS("BadAlloc", testAssert.m_msg.c_str());
		return;
	}
	FAIL("failed");
}
#endif
