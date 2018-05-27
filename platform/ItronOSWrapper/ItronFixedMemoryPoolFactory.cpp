#include "ItronFixedMemoryPoolFactory.h"
#include "private/ItronLock.h"

namespace ItronOSWrapper {

ItronFixedMemoryPoolFactory::ItronFixedMemoryPool::~ItronFixedMemoryPool()
{
	if (isCreated()) {
		deinit();
	}
}

bool ItronFixedMemoryPoolFactory::ItronFixedMemoryPool::init(std::size_t blockSize, std::size_t memoryPoolSize, void* memoryPoolAddress)
{
	T_CMPF cmpf = {0};
	cmpf.mpfatr = TA_TFIFO;
	const std::size_t oneBlockMemorySize = TSZ_MPF(1, blockSize);
	cmpf.blkcnt = memoryPoolSize / oneBlockMemorySize;
	cmpf.blksz = blockSize;
	cmpf.mpf = memoryPoolAddress;

	ER_ID mpf = acre_mpf(&cmpf);
	if (mpf <= 0) {
		return false;
	}
	m_mpfId = mpf;
	m_blockSize = blockSize;
	return true;
}

void ItronFixedMemoryPoolFactory::ItronFixedMemoryPool::deinit()
{
	del_mpf(m_mpfId);
	m_mpfId = 0;
	m_blockSize = 0U;
}

void* ItronFixedMemoryPoolFactory::ItronFixedMemoryPool::allocate()
{
	VP p = 0;
	ER err = pget_mpf(m_mpfId, &p);
	if (err != E_OK) {
		return 0;
	}
	return p;
}

void ItronFixedMemoryPoolFactory::ItronFixedMemoryPool::deallocate(void* p)
{
	if (p != 0) {
		rel_mpf(m_mpfId, p);
	}
}


ItronFixedMemoryPoolFactory::ItronFixedMemoryPoolFactory()
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

ItronFixedMemoryPoolFactory::~ItronFixedMemoryPoolFactory()
{
	if (m_mutexId > 0) {
		del_mtx(m_mutexId);
	}
}

OSWrapper::FixedMemoryPool* ItronFixedMemoryPoolFactory::create(std::size_t blockSize, std::size_t memoryPoolSize, void* memoryPoolAddress)
{
	if ((blockSize == 0U) || (memoryPoolSize == 0U)) {
		return 0;
	}
	OSWrapper::FixedMemoryPool* ret = 0;
	Lock lk(m_mutexId);
	for (FixedMemoryPoolArray::iterator it = m_pools.begin(); it != m_pools.end(); ++it) {
		if (!it->isCreated()) {
			if (it->init(blockSize, memoryPoolSize, memoryPoolAddress)) {
				ret = &*it;
			}
			break;
		}
	}
	return ret;
}

void ItronFixedMemoryPoolFactory::destroy(OSWrapper::FixedMemoryPool* p)
{
	Lock lk(m_mutexId);
	ItronFixedMemoryPool* x = static_cast<ItronFixedMemoryPool*>(p);
	x->deinit();
}

std::size_t ItronFixedMemoryPoolFactory::getRequiredMemorySize(std::size_t blockSize, std::size_t numBlocks)
{
	return TSZ_MPF(numBlocks, blockSize);
}

}
