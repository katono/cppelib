#include <exception>
#include "PlatformOSWrapperTest/PlatformOSWrapperTestHelper.h"
#if defined(PLATFORM_OS_WINDOWS)
#include "PlatformOSWrapperTest/WindowsTestHelper.h"
#elif defined(PLATFORM_OS_POSIX)
#include "PlatformOSWrapperTest/PosixTestHelper.h"
#elif defined(PLATFORM_OS_STDCPP)
#include "PlatformOSWrapperTest/StdCppTestHelper.h"
#endif

#include "CppUTest/CommandLineTestRunner.h"

int main(int argc, char **argv)
{
#if defined(PLATFORM_OS_WINDOWS)
	WindowsTestHelper helper;
	PlatformOSWrapperTestHelper::registerTestHelper(&helper);
#elif defined(PLATFORM_OS_POSIX)
	PosixTestHelper helper;
	PlatformOSWrapperTestHelper::registerTestHelper(&helper);
#elif defined(PLATFORM_OS_STDCPP)
	StdCppTestHelper helper;
	PlatformOSWrapperTestHelper::registerTestHelper(&helper);
#endif

#ifndef CPPELIB_NO_EXCEPTIONS
	try {
#endif
		return CommandLineTestRunner::RunAllTests(argc, argv);
#ifndef CPPELIB_NO_EXCEPTIONS
	}
	catch (const std::exception& e) {
		ConsoleTestOutput().printBuffer(StringFromFormat("\n%s\n", e.what()).asCharString());
	}
	catch (...) {
		ConsoleTestOutput().printBuffer("Unknown Exception\n");
	}
	return -1;
#endif
}
