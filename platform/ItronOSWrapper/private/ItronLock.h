#ifndef ITRON_LOCK_H_INCLUDED
#define ITRON_LOCK_H_INCLUDED

#include "kernel.h"

namespace ItronOSWrapper {

class Lock {
private:
	ID m_mtxId;
	ER m_err;
public:
	explicit Lock(ID mtxId) : m_mtxId(mtxId), m_err(E_OK)
	{
		if (m_mtxId > 0) {
			m_err = loc_mtx(m_mtxId);
		}
	}

	~Lock()
	{
		if ((m_mtxId > 0) && (m_err == E_OK)) {
			unl_mtx(m_mtxId);
		}
	}
};

}

#endif // ITRON_LOCK_H_INCLUDED
