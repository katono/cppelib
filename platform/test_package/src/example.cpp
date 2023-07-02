#include "OSWrapper/Thread.h"

#if defined(PLATFORM_OS_WINDOWS)
#include "WindowsOSWrapper/WindowsOSWrapper.h"
#elif defined(PLATFORM_OS_POSIX)
#include "PosixOSWrapper/PosixOSWrapper.h"
#endif
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


int main()
{
#if defined(PLATFORM_OS_WINDOWS)
	WindowsOSWrapper::init(1, 9);
#elif defined(PLATFORM_OS_POSIX)
	PosixOSWrapper::init(1, 9);
#endif
	TestRunnable r;
	OSWrapper::Thread* t = OSWrapper::Thread::create(&r);
	t->start();
	t->wait();
	std::printf("Thread finished\n");
	OSWrapper::Thread::destroy(t);
	return 0;
}
