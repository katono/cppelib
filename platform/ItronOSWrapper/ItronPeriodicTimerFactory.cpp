#include "ItronPeriodicTimerFactory.h"
#include "OSWrapper/PeriodicTimer.h"
#include "OSWrapper/Runnable.h"
#include "private/ItronLock.h"
#include <new>

namespace ItronOSWrapper {

class ItronPeriodicTimer : public OSWrapper::PeriodicTimer {
private:
	unsigned long m_periodInMillis;
	const char* m_name;

	ID m_timerId;

	static void timerCallback(void* arg)
	{
		ItronPeriodicTimer* t = static_cast<ItronPeriodicTimer*>(arg);
		if (t != 0) {
			t->timerMain();
		}
	}

public:
	ItronPeriodicTimer(OSWrapper::Runnable* r, unsigned long periodInMillis, const char* name)
	: PeriodicTimer(r), m_periodInMillis(periodInMillis), m_name(name)
	, m_timerId(0)
	{
		T_CCYC ccyc = {0};
		ccyc.cycatr = TA_HLNG;
		ccyc.exinf = this;
		ccyc.cychdr = (FP) &timerCallback;
		ccyc.cyctim = periodInMillis;
		ccyc.cycphs = 0U;;
		ER_ID cyc = acre_cyc(&ccyc);
		if (cyc <= 0) {
			return;
		}
		m_timerId = cyc;
	}

	~ItronPeriodicTimer()
	{
		if (isCreated()) {
			del_cyc(m_timerId);
		}
	}

	bool isCreated() { return m_timerId > 0; }

	ID getId() { return m_timerId; }

	void start()
	{
		sta_cyc(m_timerId);
	}

	void stop()
	{
		stp_cyc(m_timerId);
	}

	bool isStarted() const
	{
		T_RCYC rcyc = {0};
		ER err = ref_cyc(m_timerId, &rcyc);
		if (err != E_OK) {
			return false;
		}
		return rcyc.cycstat == TCYC_STA;
	}

	unsigned long getPeriodInMillis() const
	{
		return m_periodInMillis;
	}

	void setName(const char* name)
	{
		m_name = name;
	}

	const char* getName() const
	{
		return m_name;
	}

};


ItronPeriodicTimerFactory::ItronPeriodicTimerFactory()
: m_mpfId(0), m_mtxId(0), m_timerIdVec()
{
	T_CMPF cmpf = {0};
	cmpf.mpfatr = TA_TFIFO;
	cmpf.blkcnt = MAX_PERIODIC_TIMERS;
	cmpf.blksz = sizeof(ItronPeriodicTimer);
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

ItronPeriodicTimerFactory::~ItronPeriodicTimerFactory()
{
	if (m_mpfId > 0) {
		del_mpf(m_mpfId);
	}
	if (m_mtxId > 0) {
		del_mtx(m_mtxId);
	}
}

OSWrapper::PeriodicTimer* ItronPeriodicTimerFactory::create(OSWrapper::Runnable* r, unsigned long periodInMillis, const char* name)
{
	Lock lk(m_mtxId);
	if (m_mpfId <= 0) {
		return 0;
	}

	if (m_timerIdVec.full()) {
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

	ItronPeriodicTimer* t = new(p) ItronPeriodicTimer(r, periodInMillis, name);
	if (!t->isCreated()) {
		t->~ItronPeriodicTimer();
		rel_mpf(m_mpfId, t);
		return 0;
	}

	m_timerIdVec.push_back(t->getId());

	return t;
}

void ItronPeriodicTimerFactory::destroy(OSWrapper::PeriodicTimer* t)
{
	ItronPeriodicTimer* timer = static_cast<ItronPeriodicTimer*>(t);
	timer->stop();
	Lock lk(m_mtxId);
	for (TimerIDVector::iterator i = m_timerIdVec.begin(); i != m_timerIdVec.end(); ++i) {
		if (*i == timer->getId()) {
			m_timerIdVec.erase(i);
			break;
		}
	}
	timer->~ItronPeriodicTimer();
	rel_mpf(m_mpfId, timer);
}

}
