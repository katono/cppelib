#include "EventFlag.h"
#include "EventFlagFactory.h"
#include "Assertion/Assertion.h"

namespace OSWrapper {

static EventFlagFactory* s_factory = 0;

void registerEventFlagFactory(EventFlagFactory* factory)
{
	s_factory = factory;
}

EventFlag* EventFlag::create(bool autoReset)
{
	CHECK_ASSERT(s_factory);
	return s_factory->create(autoReset);
}

void EventFlag::destroy(EventFlag* e)
{
	if ((s_factory != 0) && (e != 0)) {
		s_factory->destroy(e);
	}
}

}
