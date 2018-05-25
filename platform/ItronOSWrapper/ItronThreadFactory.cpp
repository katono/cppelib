#include "ItronThreadFactory.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/Runnable.h"
#include "private/ItronLock.h"
#include <new>

#ifndef DEFAULT_STACK_SIZE
#define DEFAULT_STACK_SIZE (256U)
#endif

namespace ItronOSWrapper {

class ItronThread : public OSWrapper::Thread {
private:
	int m_priority;
	int m_initialPriority;
	std::size_t m_stackSize;
	const char* m_name;

	ID m_taskId;
	ID m_evWait;
	ID m_mtxId;

	static const FLGPTN EV_STARTED  = 0x0001U;
	static const FLGPTN EV_FINISHED = 0x0002U;

	int getTaskBasePriority(ID taskId) const
	{
		T_RTSK rtsk = {0};
		ER err = ref_tsk(taskId, &rtsk);
		if ((err != E_OK) || (rtsk.tskstat == TTS_DMT)) {
			return 0;
		}
		return rtsk.tskbpri;
	}

	int getInheritPriority(int priority) const
	{
		if ((Thread::getMinPriority() <= priority) && (priority <= Thread::getMaxPriority())) {
			return priority;
		}

		// INHERIT_PRIORITY
		return getTaskBasePriority(TSK_SELF);
	}

	bool isDormant() const
	{
		T_RTST rtst = {0};
		ER err = ref_tst(m_taskId, &rtst);
		if (err != E_OK) {
			return false;
		}
		if (rtst.tskstat == TTS_DMT) {
			return true;
		}
		return false;
	}

	void waitUntilDormant()
	{
		while (true) {
			if (isDormant()) {
				break;
			}
			dly_tsk(0U);
		}
	}

	static void threadEntry(void* arg)
	{
		ItronThread* t = static_cast<ItronThread*>(arg);
		if (t != 0) {
			t->ItronThreadMain();
		}
	}

	void ItronThreadMain()
	{
		FLGPTN ptn = 0U;
		wai_flg(m_evWait, EV_STARTED, TWF_ORW, &ptn);
		clr_flg(m_evWait, ~EV_STARTED);

		threadMain();
		set_flg(m_evWait, EV_FINISHED);
	}

public:
	ItronThread(OSWrapper::Runnable* r, int priority, std::size_t stackSize, void* stackAddress, const char* name)
	: Thread(r), m_priority(priority), m_initialPriority(priority), m_stackSize(stackSize), m_name(name), 
	  m_taskId(0), m_evWait(0), m_mtxId(0)
	{
		m_initialPriority = getInheritPriority(m_initialPriority);
		m_priority = m_initialPriority;

		T_CTSK ctsk = {0};
		ctsk.tskatr = TA_HLNG;
		ctsk.exinf = 0;
		ctsk.task = (FP) &threadEntry;
		ctsk.itskpri = m_initialPriority;
		if (m_stackSize == 0U) {
			m_stackSize = DEFAULT_STACK_SIZE;
		}
		ctsk.stksz = m_stackSize;
		ctsk.stk = stackAddress;
		ER_ID tsk = acre_tsk(&ctsk);
		if (tsk <= 0) {
			return;
		}

		T_CFLG cflg = { TA_TPRI | TA_WMUL, EV_FINISHED };
		ER_ID flg = acre_flg(&cflg);
		if (flg <= 0) {
			del_tsk(tsk);
			return;
		}

		T_CMTX cmtx = {0};
		cmtx.mtxatr = TA_INHERIT;
		ER_ID mtx = acre_mtx(&cmtx);
		if (mtx <= 0) {
			del_flg(flg);
			del_tsk(tsk);
			return;
		}

		m_taskId = tsk;
		m_evWait = flg;
		m_mtxId = mtx;
	}

	~ItronThread()
	{
		if (isCreated()) {
			del_tsk(m_taskId);
			del_flg(m_evWait);
			del_mtx(m_mtxId);
		}
	}

	bool isCreated() { return m_taskId > 0; }

	ID getId() { return m_taskId; }

	void start()
	{
		{
			Lock lk(m_mtxId);
			if (!isFinished()) {
				return;
			}
			clr_flg(m_evWait, ~EV_FINISHED);
			ER err = sta_tsk(m_taskId, this);
			if (err != E_OK) {
				return;
			}
			const int basePrio = getTaskBasePriority(m_taskId);
			if (basePrio != m_priority) {
				chg_pri(m_taskId, m_priority);
			}
		}
		set_flg(m_evWait, EV_STARTED);
	}

	OSWrapper::Error wait()
	{
		return timedWait(OSWrapper::Timeout::FOREVER);
	}

	OSWrapper::Error tryWait()
	{
		return timedWait(OSWrapper::Timeout::POLLING);
	}

	OSWrapper::Error timedWait(OSWrapper::Timeout tmout)
	{
		FLGPTN ptn = 0U;
		ER err = twai_flg(m_evWait, EV_FINISHED, TWF_ORW, &ptn, static_cast<TMO>(tmout));
		switch (err) {
		case E_OK:
			waitUntilDormant();
			return OSWrapper::OK;
		case E_TMOUT:
			return OSWrapper::TimedOut;
		default:
			return OSWrapper::OtherError;
		}
	}

	bool isFinished() const
	{
		T_RFLG rflg = {0};
		ER err = ref_flg(m_evWait, &rflg);
		if (err != E_OK) {
			return false;
		}
		if (((rflg.flgptn & EV_FINISHED) != 0U) && isDormant()) {
			return true;
		}
		return false;
	}

	void setName(const char* name)
	{
		Lock lk(m_mtxId);
		m_name = name;
	}

	const char* getName() const
	{
		Lock lk(m_mtxId);
		return m_name;
	}

	void setPriority(int priority)
	{
		Lock lk(m_mtxId);
		m_priority = getInheritPriority(priority);
		if (!isFinished()) {
			chg_pri(m_taskId, m_priority);
		}
	}

	int getPriority() const
	{
		Lock lk(m_mtxId);
		return m_priority;
	}

	int getInitialPriority() const
	{
		return m_initialPriority;
	}

	std::size_t getStackSize() const
	{
		return m_stackSize;
	}

	void* getNativeHandle()
	{
		return (void*) m_taskId;
	}

};


ItronThreadFactory::ItronThreadFactory()
: m_mpfId(0), m_mtxId(0), m_threadIdMap()
{
	T_CMPF cmpf = {0};
	cmpf.mpfatr = TA_TFIFO;
	cmpf.blkcnt = MAX_THREADS;
	cmpf.blksz = sizeof(ItronThread);
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

ItronThreadFactory::~ItronThreadFactory()
{
	if (m_mpfId > 0) {
		del_mpf(m_mpfId);
	}
	if (m_mtxId > 0) {
		del_mtx(m_mtxId);
	}
}

OSWrapper::Thread* ItronThreadFactory::create(OSWrapper::Runnable* r, int priority, std::size_t stackSize, void* stackAddress, const char* name)
{
	Lock lk(m_mtxId);
	if (m_mpfId <= 0) {
		return 0;
	}

	if (m_threadIdMap.full()) {
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

	ItronThread* t = new(p) ItronThread(r, priority, stackSize, stackAddress, name);
	if (!t->isCreated()) {
		t->~ItronThread();
		rel_mpf(m_mpfId, t);
		return 0;
	}

	IDMap map;
	map.tid = t->getId();
	map.thread = t;
	m_threadIdMap.push_back(map);

	return t;
}

void ItronThreadFactory::destroy(OSWrapper::Thread* t)
{
	ItronThread* thread = static_cast<ItronThread*>(t);
	thread->wait();
	Lock lk(m_mtxId);
	for (IDMapVector::iterator i = m_threadIdMap.begin(); i != m_threadIdMap.end(); ++i) {
		if (i->tid == thread->getId()) {
			m_threadIdMap.erase(i);
			break;
		}
	}
	thread->~ItronThread();
	rel_mpf(m_mpfId, thread);
}

void ItronThreadFactory::sleep(unsigned long millis)
{
	dly_tsk(millis);
}

void ItronThreadFactory::yield()
{
	dly_tsk(0U);
}

OSWrapper::Thread* ItronThreadFactory::getCurrentThread()
{
	ID tid = 0;
	get_tid(&tid);
	Lock lk(m_mtxId);
	for (IDMapVector::iterator i = m_threadIdMap.begin(); i != m_threadIdMap.end(); ++i) {
		if (i->tid == tid) {
			return i->thread;
		}
	}
	return 0;
}

int ItronThreadFactory::getMaxPriority() const
{
	return TMAX_TPRI;
}

int ItronThreadFactory::getMinPriority() const
{
	return TMIN_TPRI;
}

}
