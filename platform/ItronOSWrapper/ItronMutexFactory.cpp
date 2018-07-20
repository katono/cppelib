#include "ItronMutexFactory.h"
#include "OSWrapper/Mutex.h"
#include "private/ItronLock.h"
#include <cstddef>
#include <new>

#ifndef MAX_MUTEXES
#define MAX_MUTEXES (256U)
#endif

namespace ItronOSWrapper {

class ItronMutex : public OSWrapper::Mutex {
private:
	ID m_mutexId;
	std::size_t m_recursiveCount;

	bool isLockedByThisThread()
	{
		ID tskId = 0;
		ER err = get_tid(&tskId);
		if (err != E_OK) {
			return false;
		}

		T_RMTX rmtx = {0};
		err = ref_mtx(m_mutexId, &rmtx);
		if (err != E_OK) {
			return false;
		}

		if (tskId == rmtx.htskid) {
			return true;
		}
		return false;
	}

public:
	static const int INHERIT;

	explicit ItronMutex(int priorityCeilingOrInherit)
	: m_mutexId(0), m_recursiveCount(0U)
	{
		T_CMTX cmtx = {0};
		if (priorityCeilingOrInherit == INHERIT) {
			cmtx.mtxatr = TA_INHERIT;
		} else {
			cmtx.mtxatr = TA_CEILING;
			cmtx.ceilpri = priorityCeilingOrInherit;
		}
		ER_ID mtx = acre_mtx(&cmtx);
		if (mtx <= 0) {
			return;
		}
		m_mutexId = mtx;
	}

	~ItronMutex()
	{
		if (isCreated()) {
			del_mtx(m_mutexId);
		}
	}

	bool isCreated() { return m_mutexId > 0; }

	OSWrapper::Error lock()
	{
		return timedLock(OSWrapper::Timeout::FOREVER);
	}

	OSWrapper::Error tryLock()
	{
		return timedLock(OSWrapper::Timeout::POLLING);
	}

	OSWrapper::Error timedLock(OSWrapper::Timeout tmout)
	{
		ER err = tloc_mtx(m_mutexId, static_cast<TMO>(tmout));
		switch (err) {
		case E_OK:
			return OSWrapper::OK;
		case E_TMOUT:
			return OSWrapper::TimedOut;
		case E_CTX:
			return OSWrapper::CalledByNonThread;
		case E_PAR:
			return OSWrapper::InvalidParameter;
		case E_ILUSE:
			if (isLockedByThisThread()) {
				++m_recursiveCount;
				return OSWrapper::OK;
			}
			return OSWrapper::OtherError;
		default:
			return OSWrapper::OtherError;
		}
	}

	OSWrapper::Error unlock()
	{
		if (isLockedByThisThread()) {
			if (m_recursiveCount > 0U) {
				--m_recursiveCount;
				return OSWrapper::OK;
			}
		}
		ER err = unl_mtx(m_mutexId);
		switch (err) {
		case E_OK:
			return OSWrapper::OK;
		case E_ILUSE:
			return OSWrapper::NotLocked;
		default:
			return OSWrapper::OtherError;
		}
	}

};

const int ItronMutex::INHERIT = -1;

ItronMutexFactory::ItronMutexFactory()
: m_mpfId(0), m_mtxId(0)
{
	T_CMPF cmpf = {0};
	cmpf.mpfatr = TA_TFIFO;
	cmpf.blkcnt = MAX_MUTEXES;
	cmpf.blksz = sizeof(ItronMutex);
	cmpf.mpf = 0;

	ER_ID mpf = acre_mpf(&cmpf);
	if (mpf <= 0) {
		return;
	}
	m_mpfId = mpf;

	T_CMTX cmtx = {0};
	cmtx.mtxatr = TA_INHERIT;
	ER_ID mtxId = acre_mtx(&cmtx);
	if (mtxId <= 0) {
		return;
	}
	m_mtxId = mtxId;
}

ItronMutexFactory::~ItronMutexFactory()
{
	if (m_mpfId > 0) {
		del_mpf(m_mpfId);
	}
	if (m_mtxId > 0) {
		del_mtx(m_mtxId);
	}
}

OSWrapper::Mutex* ItronMutexFactory::create()
{
	return create(ItronMutex::INHERIT);
}

OSWrapper::Mutex* ItronMutexFactory::create(int priorityCeiling)
{
	Lock lk(m_mtxId);
	if (m_mpfId <= 0) {
		return 0;
	}

	VP p = 0;
	ER err = pget_mpf(m_mpfId, &p);
	if (err != E_OK) {
		return 0;
	}
	if (p == 0) {
		return 0;
	}

	ItronMutex* m = new(p) ItronMutex(priorityCeiling);
	if (!m->isCreated()) {
		m->~ItronMutex();
		rel_mpf(m_mpfId, m);
		return 0;
	}

	return m;
}

void ItronMutexFactory::destroy(OSWrapper::Mutex* m)
{
	Lock lk(m_mtxId);
	static_cast<ItronMutex*>(m)->~ItronMutex();
	rel_mpf(m_mpfId, m);
}

}
