#include "ItronVariableMemoryPoolFactory.h"
#include "private/ItronLock.h"

namespace ItronOSWrapper {

ItronVariableMemoryPoolFactory::ItronVariableMemoryPool::~ItronVariableMemoryPool()
{
	if (isCreated()) {
		deinit();
	}
}

bool ItronVariableMemoryPoolFactory::ItronVariableMemoryPool::init(std::size_t memoryPoolSize, void* memoryPoolAddress)
{
	T_CMPL cmpl = {0};
	cmpl.mplatr = TA_TFIFO;
	cmpl.mplsz = memoryPoolSize;
	cmpl.mpl = memoryPoolAddress;

	ER_ID mpl = acre_mpl(&cmpl);
	if (mpl <= 0) {
		return false;
	}
	m_mplId = mpl;
	return true;
}

void ItronVariableMemoryPoolFactory::ItronVariableMemoryPool::deinit()
{
	del_mpl(m_mplId);
	m_mplId = 0;
}

void* ItronVariableMemoryPoolFactory::ItronVariableMemoryPool::allocate(std::size_t size)
{
	VP p = 0;
	ER err = pget_mpl(m_mplId, size, &p);
	if (err != E_OK) {
		return 0;
	}
	return p;
}

void ItronVariableMemoryPoolFactory::ItronVariableMemoryPool::deallocate(void* p)
{
	if (p != 0) {
		rel_mpl(m_mplId, p);
	}
}


ItronVariableMemoryPoolFactory::ItronVariableMemoryPoolFactory()
: m_pools(), m_mutexId(0)
{
	T_CMTX cmtx = {0};
	cmtx.mtxatr = TA_INHERIT;
	ER_ID mtx = acre_mtx(&cmtx);
	if (mtx <= 0) {
		return;
	}
	m_mutexId = mtx;
}

ItronVariableMemoryPoolFactory::~ItronVariableMemoryPoolFactory()
{
	if (m_mutexId > 0) {
		del_mtx(m_mutexId);
	}
}

OSWrapper::VariableMemoryPool* ItronVariableMemoryPoolFactory::create(std::size_t memoryPoolSize, void* memoryPoolAddress)
{
	if (memoryPoolSize == 0U) {
		return 0;
	}
	OSWrapper::VariableMemoryPool* ret = 0;
	Lock lk(m_mutexId);
	for (VariableMemoryPoolArray::iterator it = m_pools.begin(); it != m_pools.end(); ++it) {
		if (!it->isCreated()) {
			if (it->init(memoryPoolSize, memoryPoolAddress)) {
				ret = &*it;
			}
			break;
		}
	}
	return ret;
}

void ItronVariableMemoryPoolFactory::destroy(OSWrapper::VariableMemoryPool* p)
{
	Lock lk(m_mutexId);
	ItronVariableMemoryPool* x = static_cast<ItronVariableMemoryPool*>(p);
	x->deinit();
}

}
