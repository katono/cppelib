#ifndef CONTAINER_EXCEPTION_H_INCLUDED
#define CONTAINER_EXCEPTION_H_INCLUDED

#include <exception>

//! @cond
#ifdef CPPELIB_NO_EXCEPTIONS
#include "Assertion/Assertion.h"
#define CPPELIB_CONTAINER_THROW(x) CHECK_ASSERT(false && #x)
#else
#define CPPELIB_CONTAINER_THROW(x) throw x
#endif

#if (__cplusplus >= 201103L)
#define CPPELIB_CONTAINER_NOEXCEPT noexcept
#else
#define CPPELIB_CONTAINER_NOEXCEPT throw()
#endif
//! @endcond

namespace Container {

class OutOfRange : public std::exception {
public:
	explicit OutOfRange(const char* msg) : m_msg(msg) {}
	const char* what() const CPPELIB_CONTAINER_NOEXCEPT
	{
		return m_msg;
	}
private:
	const char* m_msg;
};

class BadAlloc : public std::exception {
public:
	BadAlloc() {}
	virtual const char* what() const CPPELIB_CONTAINER_NOEXCEPT = 0;
};

}

#endif // CONTAINER_EXCEPTION_H_INCLUDED
