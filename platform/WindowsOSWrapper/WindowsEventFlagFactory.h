#ifndef WINDOWS_OS_WRAPPER_WINDOWS_EVENT_FLAG_FACTORY_H_INCLUDED
#define WINDOWS_OS_WRAPPER_WINDOWS_EVENT_FLAG_FACTORY_H_INCLUDED

#include "OSWrapper/EventFlagFactory.h"
#include <mutex>

namespace WindowsOSWrapper {

class WindowsEventFlagFactory : public OSWrapper::EventFlagFactory {
public:
	WindowsEventFlagFactory();
	virtual ~WindowsEventFlagFactory() {}

private:
	virtual OSWrapper::EventFlag* create(bool autoReset);
	virtual void destroy(OSWrapper::EventFlag* e);

	std::mutex m_mutex;
};

}

#endif // WINDOWS_OS_WRAPPER_WINDOWS_EVENT_FLAG_FACTORY_H_INCLUDED
