#include "Mutex.h"
#include "MutexFactory.h"
#include "Assertion/Assertion.h"

namespace OSWrapper {

static MutexFactory* s_factory = 0;

void registerMutexFactory(MutexFactory* factory)
{
	s_factory = factory;
}

Mutex* Mutex::create()
{
	CHECK_PRECOND(s_factory);
	return s_factory->create();
}

Mutex* Mutex::create(int priorityCeiling)
{
	CHECK_PRECOND(s_factory);
	return s_factory->create(priorityCeiling);
}

void Mutex::destroy(Mutex* m)
{
	if ((s_factory != 0) && (m != 0)) {
		s_factory->destroy(m);
	}
}


LockGuard::LockGuard(Mutex* m)
: m_mutex(m)
{
	if (m_mutex != 0) {
		m_mutex->lock();
	}
}

LockGuard::~LockGuard()
{
	if (m_mutex != 0) {
		m_mutex->unlock();
	}
}

}
