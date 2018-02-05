#ifndef OS_WRAPPER_EVENT_FLAG_FACTORY_H_INCLUDED
#define OS_WRAPPER_EVENT_FLAG_FACTORY_H_INCLUDED

namespace OSWrapper {

class EventFlag;

class EventFlagFactory {
public:
	virtual ~EventFlagFactory() {}
	virtual EventFlag* create(bool autoReset) = 0;
	virtual void destroy(EventFlag* e) = 0;
};

}

#endif // OS_WRAPPER_EVENT_FLAG_FACTORY_H_INCLUDED
