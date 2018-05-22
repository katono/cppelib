#include "ItronThreadFactory.h"
#include "OSWrapper/Thread.h"
#include "OSWrapper/Runnable.h"
#include "private/ItronLock.h"
#include <new>

namespace ItronOSWrapper {

class ItronThread : public OSWrapper::Thread {
private:
	int m_priority;
	std::size_t m_stackSize;
	const char* m_name;

	ID m_taskId;
	ID m_evWait;

	static ID m_mtxId;

	int getInheritPriority(int priority)
	{
		if ((Thread::getMinPriority() <= priority) && (priority <= Thread::getMaxPriority())) {
			return priority;
		}

		// INHERIT_PRIORITY
		PRI this_priority;
		ER err = get_pri(TSK_SELF, &this_priority);
		if (err != E_OK) {
			return priority;
		}
		return this_priority;
	}

	void waitUntilDormant()
	{
		while (true) {
			T_RTST rtst = {0};
			ER err = ref_tst(m_taskId, &rtst);
			if (err != E_OK) {
				break;
			}
			if (rtst.tskstat == TTS_DMT) {
				break;
			}
			dly_tsk(0U);
		}
	}

public:
	static void setMutex(ID mtxId) { m_mtxId = mtxId; }

	static void threadEntry(void* arg)
	{
		ItronThread* t = static_cast<ItronThread*>(arg);
		if (t != 0) {
			t->ItronThreadMain();
		}
	}

	void ItronThreadMain()
	{
		threadMain();
		set_flg(m_evWait, 1U);
	}

	ItronThread(OSWrapper::Runnable* r, int priority, std::size_t stackSize, void* stackAddress, const char* name)
	: Thread(r), m_priority(priority), m_stackSize(stackSize), m_name(name), 
	  m_taskId(0), m_evWait(0)
	{
		m_priority = getInheritPriority(m_priority);

		T_CTSK ctsk = {0};
		ctsk.tskatr = TA_HLNG;
		ctsk.exinf = 0;
		ctsk.task = (FP) &threadEntry;
		ctsk.itskpri = m_priority;
		if (m_stackSize == 0U) {
			m_stackSize = 256U;
		}
		ctsk.stksz = m_stackSize;
		ctsk.stk = stackAddress;
		ER_ID tsk = acre_tsk(&ctsk);
		if (tsk <= 0) {
			return;
		}

		T_CFLG cflg = { TA_TPRI | TA_WMUL, 1U };
		ER_ID flg = acre_flg(&cflg);
		if (flg <= 0) {
			del_tsk(tsk);
			return;
		}
		m_taskId = tsk;
		m_evWait = flg;
	}

	~ItronThread()
	{
		if (isCreated()) {
			del_tsk(m_taskId);
			del_flg(m_evWait);
		}
	}

	bool isCreated() { return m_taskId > 0; }

	ID getId() { return m_taskId; }

	void start()
	{
		Lock lk(m_mtxId);
		ER err = sta_tsk(m_taskId, this);
		if (err != E_OK) {
			return;
		}
		chg_pri(m_taskId, m_priority);
		clr_flg(m_evWait, 0U);
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
		ER err = twai_flg(m_evWait, 1U, TWF_ORW, &ptn, static_cast<TMO>(tmout));
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
		Lock lk(m_mtxId);
		T_RFLG rflg = {0};
		ER err = ref_flg(m_evWait, &rflg);
		if (err != E_OK) {
			return true;
		}
		return rflg.flgptn != 0U;
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
		T_RTSK rtsk = {0};
		ER err = ref_tsk(m_taskId, &rtsk);
		if (err != E_OK) {
			return 0;
		}
		return rtsk.tskbpri;
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

ID ItronThread::m_mtxId = 0;


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

	ItronThread::setMutex(m_mtxId);
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
