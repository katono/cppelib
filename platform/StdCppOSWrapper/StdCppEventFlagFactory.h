#ifndef STDCPP_OS_WRAPPER_STDCPP_EVENT_FLAG_FACTORY_H_INCLUDED
#define STDCPP_OS_WRAPPER_STDCPP_EVENT_FLAG_FACTORY_H_INCLUDED

#include "OSWrapper/EventFlagFactory.h"
#include <mutex>

namespace StdCppOSWrapper {

class StdCppEventFlagFactory : public OSWrapper::EventFlagFactory {
public:
	StdCppEventFlagFactory();
	virtual ~StdCppEventFlagFactory() {}

private:
	virtual OSWrapper::EventFlag* create(bool autoReset);
	virtual void destroy(OSWrapper::EventFlag* e);

	StdCppEventFlagFactory(const StdCppEventFlagFactory&);
	StdCppEventFlagFactory& operator=(const StdCppEventFlagFactory&);

	std::mutex m_mutex;
};

}

#endif // STDCPP_OS_WRAPPER_STDCPP_EVENT_FLAG_FACTORY_H_INCLUDED
