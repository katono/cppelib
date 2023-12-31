#include <exception>
#include "CppUTest/CommandLineTestRunner.h"

int main(int argc, char **argv)
{
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
