#ifndef OS_WRAPPER_ERROR_H_INCLUDED
#define OS_WRAPPER_ERROR_H_INCLUDED

namespace OSWrapper {

enum Error {
	OK,
	TimedOut,
	CalledByNonThread,
	InvalidParameter,
	LockedRecursively,
	NotLocked,
	OtherThreadWaiting,
	OtherError
};

}

#endif // OS_WRAPPER_ERROR_H_INCLUDED