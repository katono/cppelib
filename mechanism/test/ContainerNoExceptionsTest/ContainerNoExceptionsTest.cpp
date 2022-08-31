#include "Assertion/Assertion.h"
#include "Container/Array.h"
#include "Container/FixedVector.h"
#include "Container/FixedDeque.h"
#include "Container/PreallocatedVector.h"
#include "Container/PreallocatedDeque.h"
#include <string>
#include <csetjmp>
#include <cstdio>

using Container::Array;
using Container::FixedVector;
using Container::FixedDeque;
using Container::PreallocatedVector;
using Container::PreallocatedDeque;

namespace {
std::string s_puts;
std::jmp_buf s_jmpBuf;
}

class TestAssert : public Assertion::AssertHandler {
public:
	void handle(const char* msg)
	{
		s_puts = msg;
		std::puts(msg);
		std::longjmp(s_jmpBuf, -1);
	}
};

int ArrayTest()
{
	std::string failureString("ArrayTest failed: line ");

	Array<int, 10> x;
	if (setjmp(s_jmpBuf) == 0) {
		x.at(9);
	} else {
		failureString += std::to_string(__LINE__);
		goto testFailed;
	}

	if (setjmp(s_jmpBuf) == 0) {
		x.at(10);
		failureString += std::to_string(__LINE__);
		goto testFailed;
	} else {
		if (s_puts.find("OutOfRange") == std::string::npos) {
			failureString += std::to_string(__LINE__);
			goto testFailed;
		}
		if (s_puts.find("Array::at") == std::string::npos) {
			failureString += std::to_string(__LINE__);
			goto testFailed;
		}
	}

	std::puts("ArrayTest success");
	return 0;

testFailed:
	std::puts(failureString.c_str());
	return -1;
}

template <typename C>
int ContainerTest(C&& x, const std::string& containerName)
{
	std::string successString(containerName + "Test success");
	std::string failureString(containerName + "Test failed: line ");

	if (setjmp(s_jmpBuf) == 0) {
		x.resize(x.max_size());
		x.at(x.max_size() - 1);
	} else {
		failureString += std::to_string(__LINE__);
		goto testFailed;
	}

	if (setjmp(s_jmpBuf) == 0) {
		x.at(x.max_size());
		failureString += std::to_string(__LINE__);
		goto testFailed;
	} else {
		if (s_puts.find("OutOfRange") == std::string::npos) {
			failureString += std::to_string(__LINE__);
			goto testFailed;
		}
		if (s_puts.find(containerName + "::at") == std::string::npos) {
			failureString += std::to_string(__LINE__);
			goto testFailed;
		}
	}

	if (setjmp(s_jmpBuf) == 0) {
		x.push_back(123);
		failureString += std::to_string(__LINE__);
		goto testFailed;
	} else {
		if (s_puts.find("BadAlloc") == std::string::npos) {
			failureString += std::to_string(__LINE__);
			goto testFailed;
		}
	}

	std::puts(successString.c_str());
	return 0;

testFailed:
	std::puts(failureString.c_str());
	return -1;
}

int main()
{
	static TestAssert testAssert;
	Assertion::setHandler(&testAssert);

	int ret = 0;
	ret |= ArrayTest();
	ret |= ContainerTest<FixedVector<int, 10>>(FixedVector<int, 10>(), "FixedVector");
	ret |= ContainerTest<FixedDeque<int, 10>>(FixedDeque<int, 10>(), "FixedDeque");
	int buf1[10];
	ret |= ContainerTest<PreallocatedVector<int>>(PreallocatedVector<int>(buf1, sizeof buf1), "PreallocatedVector");
	int buf2[11];
	ret |= ContainerTest<PreallocatedDeque<int>>(PreallocatedDeque<int>(buf2, sizeof buf2), "PreallocatedDeque");
	return ret;
}
