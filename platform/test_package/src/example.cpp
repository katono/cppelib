#include "OSWrapper/Thread.h"

#if defined(PLATFORM_OS_WINDOWS)
#include "WindowsOSWrapper/WindowsOSWrapper.h"
#include "StdCppOSWrapper/StdCppOSWrapper.h"
#elif defined(PLATFORM_OS_POSIX)
#include "PosixOSWrapper/PosixOSWrapper.h"
#include "StdCppOSWrapper/StdCppOSWrapper.h"
#endif
#include "TestDoubleOSWrapper/TestDoubleOSWrapper.h"

#include "OSWrapper/Runnable.h"
#include "OSWrapper/Thread.h"
#include <cstdio>

class TestRunnable : public OSWrapper::Runnable {
public:
	void run()
	{
		std::printf("Thread running\n");
	}
};

void runTest()
{
	TestRunnable r;
	OSWrapper::Thread* t = OSWrapper::Thread::create(&r);
	t->start();
	t->wait();
	std::printf("Thread finished\n");
	OSWrapper::Thread::destroy(t);
}

int main()
{
#if defined(PLATFORM_OS_WINDOWS)
	std::printf("WindowsOSWrapper\n");
	WindowsOSWrapper::init(1, 9);
	runTest();

	std::printf("StdCppOSWrapper\n");
	StdCppOSWrapper::init();
	runTest();
#elif defined(PLATFORM_OS_POSIX)
	std::printf("PosixOSWrapper\n");
	PosixOSWrapper::init(1, 9);
	runTest();

	std::printf("StdCppOSWrapper\n");
	StdCppOSWrapper::init();
	runTest();
#endif
	std::printf("TestDoubleOSWrapper\n");
	TestDoubleOSWrapper::init();
	runTest();

	return 0;
}
