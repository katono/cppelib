#include "Assertion/Assertion.h"
#include <string>
#include <csetjmp>
#include <cstdio>

namespace {
std::string s_puts;
std::jmp_buf s_jmpBuf;
}

class TestAssert : public Assertion::AssertHandler {
public:
	void handle(const char* msg)
	{
		s_puts = msg;
		// std::puts(msg);
		std::longjmp(s_jmpBuf, -1);
	}
};

int main()
{
	static TestAssert testAssert;
	Assertion::setHandler(&testAssert);

	std::string failureString("failed: line ");
	volatile int failureLine = 0;
	if (setjmp(s_jmpBuf) == 0) {
		bool a = true;
		CHECK_ASSERT(a);
	} else {
		failureString += std::to_string(__LINE__);
		goto testFailed;
	}

	if (setjmp(s_jmpBuf) == 0) {
		bool a = false;
		failureLine = __LINE__; CHECK_ASSERT(a);
		failureString += std::to_string(__LINE__);
		goto testFailed;
	} else {
		if (s_puts.find(__FILE__) == std::string::npos) {
			failureString += std::to_string(__LINE__);
			goto testFailed;
		}
		if (s_puts.find(std::to_string(failureLine)) == std::string::npos) {
			failureString += std::to_string(__LINE__);
			goto testFailed;
		}
		if (s_puts.find("Assertion failed") == std::string::npos) {
			failureString += std::to_string(__LINE__);
			goto testFailed;
		}
	}
	std::puts("success");
	return 0;

testFailed:
	std::puts(failureString.c_str());
	return -1;
}
