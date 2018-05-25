#include "ItronEventFlagFactory.h"
#include "OSWrapper/EventFlag.h"
#include "private/ItronLock.h"
#include <new>

#ifndef MAX_EVENT_FLAGS
#define MAX_EVENT_FLAGS (256U)
#endif

namespace ItronOSWrapper {

using OSWrapper::EventFlag;
using OSWrapper::Timeout;

class ItronEventFlag : public OSWrapper::EventFlag {
private:
	ID m_flgId;

public:
	ItronEventFlag(bool autoReset)
	: m_flgId(0)
	{
		T_CFLG cflg = {0};
		cflg.flgatr = TA_TPRI | TA_WMUL | (autoReset ? TA_CLR : 0);
		cflg.iflgptn = 0;
		ER_ID flg = acre_flg(&cflg);
		if (flg <= 0) {
			return;
		}
		m_flgId = flg;
	}

	~ItronEventFlag()
	{
		if (isCreated()) {
			del_flg(m_flgId);
		}
	}

	bool isCreated() { return m_flgId > 0; }

	OSWrapper::Error waitAny()
	{
		return timedWaitAny(Timeout::FOREVER);
	}

	OSWrapper::Error waitOne(std::size_t pos)
	{
		return timedWaitOne(pos, Timeout::FOREVER);
	}

	OSWrapper::Error wait(EventFlag::Pattern bitPattern, Mode waitMode, EventFlag::Pattern* releasedPattern)
	{
		return timedWait(bitPattern, waitMode, releasedPattern, Timeout::FOREVER);
	}

	OSWrapper::Error tryWaitAny()
	{
		return timedWaitAny(Timeout::POLLING);
	}

	OSWrapper::Error tryWaitOne(std::size_t pos)
	{
		return timedWaitOne(pos, Timeout::POLLING);
	}

	OSWrapper::Error tryWait(EventFlag::Pattern bitPattern, Mode waitMode, EventFlag::Pattern* releasedPattern)
	{
		return timedWait(bitPattern, waitMode, releasedPattern, Timeout::POLLING);
	}

	OSWrapper::Error timedWaitAny(Timeout tmout)
	{
		return timedWait(EventFlag::Pattern().set(), EventFlag::OR, 0, tmout);
	}

	OSWrapper::Error timedWaitOne(std::size_t pos, Timeout tmout)
	{
		if (pos >= EventFlag::Pattern().size()) {
			return OSWrapper::InvalidParameter;
		}
		return timedWait(EventFlag::Pattern().set(pos), EventFlag::OR, 0, tmout);
	}

	OSWrapper::Error timedWait(EventFlag::Pattern bitPattern, Mode waitMode, EventFlag::Pattern* releasedPattern, Timeout tmout)
	{
		if ((waitMode != EventFlag::OR) && (waitMode != EventFlag::AND)) {
			return OSWrapper::InvalidParameter;
		}
		if (bitPattern.none()) {
			return OSWrapper::InvalidParameter;
		}

		FLGPTN ptn = 0U;
		ER err = twai_flg(m_flgId, static_cast<FLGPTN>(bitPattern),
				(waitMode == EventFlag::OR) ? TWF_ORW : TWF_ANDW, &ptn, static_cast<TMO>(tmout));
		switch (err) {
		case E_OK:
			if (releasedPattern != 0) {
				*releasedPattern = EventFlag::Pattern(ptn);
			}
			return OSWrapper::OK;
		case E_TMOUT:
			return OSWrapper::TimedOut;
		case E_CTX:
			return OSWrapper::CalledByNonThread;
		case E_PAR:
			return OSWrapper::InvalidParameter;
		default:
			return OSWrapper::OtherError;
		}
	}

	OSWrapper::Error setAll()
	{
		return set(EventFlag::Pattern().set());
	}

	OSWrapper::Error setOne(std::size_t pos)
	{
		if (pos >= EventFlag::Pattern().size()) {
			return OSWrapper::InvalidParameter;
		}
		return set(EventFlag::Pattern().set(pos));
	}

	OSWrapper::Error set(EventFlag::Pattern bitPattern)
	{
		ER err = set_flg(m_flgId, static_cast<FLGPTN>(bitPattern));
		if (err == E_OK) {
			return OSWrapper::OK;
		} else {
			return OSWrapper::OtherError;
		}
	}

	OSWrapper::Error resetAll()
	{
		return reset(EventFlag::Pattern().set());
	}

	OSWrapper::Error resetOne(std::size_t pos)
	{
		if (pos >= EventFlag::Pattern().size()) {
			return OSWrapper::InvalidParameter;
		}
		return reset(EventFlag::Pattern().set(pos));
	}

	OSWrapper::Error reset(EventFlag::Pattern bitPattern)
	{
		ER err = clr_flg(m_flgId, static_cast<FLGPTN>(bitPattern.flip()));
		if (err == E_OK) {
			return OSWrapper::OK;
		} else {
			return OSWrapper::OtherError;
		}
	}

	EventFlag::Pattern getCurrentPattern() const
	{
		T_RFLG rflg = {0};
		ER err = ref_flg(m_flgId, &rflg);
		if (err == E_OK) {
			return EventFlag::Pattern(rflg.flgptn);
		} else {
			return EventFlag::Pattern(0U);
		}
	}

};


ItronEventFlagFactory::ItronEventFlagFactory()
: m_mpfId(0), m_mtxId(0)
{
	T_CMPF cmpf = {0};
	cmpf.mpfatr = TA_TFIFO;
	cmpf.blkcnt = MAX_EVENT_FLAGS;
	cmpf.blksz = sizeof(ItronEventFlag);
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

ItronEventFlagFactory::~ItronEventFlagFactory()
{
	if (m_mpfId > 0) {
		del_mpf(m_mpfId);
	}
	if (m_mtxId > 0) {
		del_mtx(m_mtxId);
	}
}

OSWrapper::EventFlag* ItronEventFlagFactory::create(bool autoReset)
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

	ItronEventFlag* e = new(p) ItronEventFlag(autoReset);
	if (!e->isCreated()) {
		e->~ItronEventFlag();
		rel_mpf(m_mpfId, e);
		return 0;
	}

	return e;
}

void ItronEventFlagFactory::destroy(OSWrapper::EventFlag* e)
{
	Lock lk(m_mtxId);
	static_cast<ItronEventFlag*>(e)->~ItronEventFlag();
	rel_mpf(m_mpfId, e);
}

}
