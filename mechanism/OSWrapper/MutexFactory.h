#ifndef OS_WRAPPER_MUTEX_FACTORY_H_INCLUDED
#define OS_WRAPPER_MUTEX_FACTORY_H_INCLUDED

namespace OSWrapper {

class Mutex;

class MutexFactory {
public:
	virtual ~MutexFactory() {}
	virtual Mutex* create() = 0;
	virtual Mutex* create(int priorityCeiling) = 0;
	virtual void destroy(Mutex* m) = 0;
};

}

#endif // OS_WRAPPER_MUTEX_FACTORY_H_INCLUDED
