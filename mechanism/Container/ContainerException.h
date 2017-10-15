#ifndef CONTAINER_EXCEPTION_H_INCLUDED
#define CONTAINER_EXCEPTION_H_INCLUDED

#include <exception>

namespace Container {

class OutOfRange : public std::exception {
public:
	explicit OutOfRange(const char* msg) : m_msg(msg) {}
	const char* what() const throw()
	{
		return m_msg;
	}
private:
	const char* m_msg;
};

class BadAlloc : public std::exception {
public:
	BadAlloc() {}
	const char* what() const throw()
	{
		return "Container::BadAlloc";
	}
};

}

#endif // CONTAINER_EXCEPTION_H_INCLUDED
