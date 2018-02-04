#include "WindowsMutexFactory.h"
#include "private/WindowsMutex.h"

namespace WindowsOSWrapper {

WindowsMutexFactory::WindowsMutexFactory()
: m_mutex()
{
}

OSWrapper::Mutex* WindowsMutexFactory::create()
{
	try {
		std::lock_guard<std::mutex> lock(m_mutex);
		WindowsMutex* m = new WindowsMutex();
		return m;
	}
	catch (...) {
		return nullptr;
	}
}

OSWrapper::Mutex* WindowsMutexFactory::create(int priorityCeiling)
{
	(void) priorityCeiling;
	return create();
}

void WindowsMutexFactory::destroy(OSWrapper::Mutex* m)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	delete m;
}

}
