#include "Mutex.h"
#include "MutexFactory.h"
#include "DesignByContract/Assertion.h"

namespace OSWrapper {

MutexFactory* Mutex::m_factory = 0;

void Mutex::setFactory(MutexFactory* factory)
{
	m_factory = factory;
}

Mutex* Mutex::create()
{
	DBC_PRE(m_factory);
	return m_factory->create();
}

Mutex* Mutex::create(int priorityCeiling)
{
	DBC_PRE(m_factory);
	return m_factory->create(priorityCeiling);
}

void Mutex::destroy(Mutex* m)
{
	if ((m_factory != 0) && (m != 0)) {
		m_factory->destroy(m);
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
