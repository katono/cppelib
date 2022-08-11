#include "Assertion/Assertion.h"
#include <string>
#include <csetjmp>
#include <cstdio>

namespace {
std::string s_puts;
std::jmp_buf s_jmpBuf;
}

int testPuts(const char* str)
{
	s_puts = str;
	std::puts(str);
	return 0;
}

void testAbort()
{
	std::longjmp(s_jmpBuf, -1);
}

int main()
{
	Assertion::UserSpecificFunction::setPuts(testPuts);
	Assertion::UserSpecificFunction::setAbort(testAbort);

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
