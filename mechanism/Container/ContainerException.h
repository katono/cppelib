#ifndef CONTAINER_EXCEPTION_H_INCLUDED
#define CONTAINER_EXCEPTION_H_INCLUDED

#include <exception>

#ifdef NO_EXCEPTIONS
#include <cstdlib>
#define CONTAINER_THROW(x) std::abort()
#else
#define CONTAINER_THROW(x) throw x
#endif

#if (__cplusplus >= 201103L)
#define CONTAINER_NOEXCEPT noexcept
#else
#define CONTAINER_NOEXCEPT throw()
#endif

namespace Container {

class OutOfRange : public std::exception {
public:
	explicit OutOfRange(const char* msg) : m_msg(msg) {}
	const char* what() const CONTAINER_NOEXCEPT
	{
		return m_msg;
	}
private:
	const char* m_msg;
};

class BadAlloc : public std::exception {
public:
	BadAlloc() {}
	virtual const char* what() const CONTAINER_NOEXCEPT = 0;
};

}

#endif // CONTAINER_EXCEPTION_H_INCLUDED
