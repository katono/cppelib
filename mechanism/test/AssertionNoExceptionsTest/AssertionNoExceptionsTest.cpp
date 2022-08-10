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
	// std::puts(str);
	return 0;
}

void testAbort()
{
	std::longjmp(s_jmpBuf, -1);
}

int main()
{
	Assertion::UserSpecificFunc::setPuts(testPuts);
	Assertion::UserSpecificFunc::setAbort(testAbort);

	int aborted = 0;
	if ((aborted = setjmp(s_jmpBuf)) == 0) {
		bool a = true;
		CHECK_ASSERT(a);
	} else if (aborted != 0) {
		goto testFailed;
	}

	int failureLine = 0;
	if ((aborted = setjmp(s_jmpBuf)) == 0) {
		bool a = false;
		failureLine = __LINE__; CHECK_ASSERT(a);
	} else if (aborted != 0) {
		if (s_puts.find(__FILE__) == std::string::npos) {
			goto testFailed;
		}
		if (s_puts.find(std::to_string(failureLine)) == std::string::npos) {
			goto testFailed;
		}
		if (s_puts.find("Assertion failed") == std::string::npos) {
			goto testFailed;
		}
		std::puts("success");
		return 0;
	}

testFailed:
	std::puts("failed");
	return -1;
}
