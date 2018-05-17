#ifndef ITRON_OS_WRAPPER_ITRON_EVENT_FLAG_FACTORY_H_INCLUDED
#define ITRON_OS_WRAPPER_ITRON_EVENT_FLAG_FACTORY_H_INCLUDED

#include "OSWrapper/EventFlagFactory.h"
#include "kernel.h"

namespace ItronOSWrapper {

class ItronEventFlagFactory : public OSWrapper::EventFlagFactory {
public:
	ItronEventFlagFactory();
	virtual ~ItronEventFlagFactory();

private:
	virtual OSWrapper::EventFlag* create(bool autoReset);
	virtual void destroy(OSWrapper::EventFlag* e);

	ItronEventFlagFactory(const ItronEventFlagFactory&);
	ItronEventFlagFactory& operator=(const ItronEventFlagFactory&);

	ID m_mpfId;
	ID m_mtxId;
};

}

#endif // ITRON_OS_WRAPPER_ITRON_EVENT_FLAG_FACTORY_H_INCLUDED
