#include "WindowsEventFlagFactory.h"
#include "private/WindowsEventFlag.h"

namespace WindowsOSWrapper {

WindowsEventFlagFactory::WindowsEventFlagFactory()
: m_mutex()
{
}

OSWrapper::EventFlag* WindowsEventFlagFactory::create(bool autoReset)
{
	try {
		std::lock_guard<std::mutex> lock(m_mutex);
		WindowsEventFlag* e = new WindowsEventFlag(autoReset);
		return e;
	}
	catch (...) {
		return nullptr;
	}
}

void WindowsEventFlagFactory::destroy(OSWrapper::EventFlag* e)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	delete static_cast<WindowsEventFlag*>(e);
}

}
