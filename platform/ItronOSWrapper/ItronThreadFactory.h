#ifndef ITRON_OS_WRAPPER_ITRON_THREAD_FACTORY_H_INCLUDED
#define ITRON_OS_WRAPPER_ITRON_THREAD_FACTORY_H_INCLUDED

#include "OSWrapper/ThreadFactory.h"
#include "Container/FixedVector.h"
#include "kernel.h"

#ifndef MAX_THREADS
#define MAX_THREADS (256U)
#endif

namespace ItronOSWrapper {

class ItronThread;

class ItronThreadFactory : public OSWrapper::ThreadFactory {
public:
	ItronThreadFactory();
	virtual ~ItronThreadFactory();

private:
	virtual OSWrapper::Thread* create(OSWrapper::Runnable* r, int priority, std::size_t stackSize, void* stackAddress, const char* name);
	virtual void destroy(OSWrapper::Thread* t);
	virtual void sleep(unsigned long millis);
	virtual void yield();
	virtual OSWrapper::Thread* getCurrentThread();
	virtual int getMaxPriority() const;
	virtual int getMinPriority() const;
	virtual int getHighestPriority() const;
	virtual int getLowestPriority() const;

	ItronThreadFactory(const ItronThreadFactory&);
	ItronThreadFactory& operator=(const ItronThreadFactory&);

	ID m_mpfId;
	ID m_mtxId;

	struct IDMap {
		ID tid;
		ItronThread* thread;
	};
	typedef Container::FixedVector<IDMap, MAX_THREADS> IDMapVector;
	IDMapVector m_threadIdMap;
};

}

#endif // ITRON_OS_WRAPPER_ITRON_THREAD_FACTORY_H_INCLUDED
