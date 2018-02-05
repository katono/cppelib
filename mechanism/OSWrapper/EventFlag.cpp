#include "EventFlag.h"
#include "EventFlagFactory.h"
#include "DesignByContract/Assertion.h"

namespace OSWrapper {

EventFlagFactory* EventFlag::m_factory = 0;

void EventFlag::setFactory(EventFlagFactory* factory)
{
	m_factory = factory;
}

EventFlag* EventFlag::create(bool autoReset)
{
	DBC_PRE(m_factory);
	return m_factory->create(autoReset);
}

void EventFlag::destroy(EventFlag* e)
{
	if ((m_factory != 0) && (e != 0)) {
		m_factory->destroy(e);
	}
}

}
