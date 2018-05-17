#ifndef ITRON_OS_WRAPPER_ITRON_MUTEX_FACTORY_H_INCLUDED
#define ITRON_OS_WRAPPER_ITRON_MUTEX_FACTORY_H_INCLUDED

#include "OSWrapper/MutexFactory.h"
#include "kernel.h"

namespace ItronOSWrapper {

class ItronMutexFactory : public OSWrapper::MutexFactory {
public:
	ItronMutexFactory();
	virtual ~ItronMutexFactory();

private:
	virtual OSWrapper::Mutex* create();
	virtual OSWrapper::Mutex* create(int priorityCeiling);
	virtual void destroy(OSWrapper::Mutex* m);

	ItronMutexFactory(const ItronMutexFactory&);
	ItronMutexFactory& operator=(const ItronMutexFactory&);

	ID m_mpfId;
	ID m_mtxId;
};

}

#endif // ITRON_OS_WRAPPER_ITRON_MUTEX_FACTORY_H_INCLUDED
