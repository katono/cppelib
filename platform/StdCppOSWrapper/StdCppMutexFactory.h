#ifndef STDCPP_OS_WRAPPER_STDCPP_MUTEX_FACTORY_H_INCLUDED
#define STDCPP_OS_WRAPPER_STDCPP_MUTEX_FACTORY_H_INCLUDED

#include "OSWrapper/MutexFactory.h"
#include <mutex>

namespace StdCppOSWrapper {

class StdCppMutexFactory : public OSWrapper::MutexFactory {
public:
	StdCppMutexFactory();
	virtual ~StdCppMutexFactory() {}

private:
	virtual OSWrapper::Mutex* create();
	virtual OSWrapper::Mutex* create(int priorityCeiling);
	virtual void destroy(OSWrapper::Mutex* m);

	StdCppMutexFactory(const StdCppMutexFactory&);
	StdCppMutexFactory& operator=(const StdCppMutexFactory&);

	std::mutex m_mutex;
};

}

#endif // STDCPP_OS_WRAPPER_STDCPP_MUTEX_FACTORY_H_INCLUDED
