#include "CppUTest/CommandLineTestRunner.h"

int main(int argc, char **argv)
{
	try {
		return CommandLineTestRunner::RunAllTests(argc, argv);
	}
	catch (const std::exception& e) {
		ConsoleTestOutput().printBuffer(StringFromFormat("\n%s\n", e.what()).asCharString());
	}
	catch (...) {
		ConsoleTestOutput().printBuffer("Unknown Exception\n");
	}
	return -1;
}
