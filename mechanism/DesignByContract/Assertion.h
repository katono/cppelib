#ifndef DESIGNBYCONTRACT_ASSERTION_H_INCLUDED
#define DESIGNBYCONTRACT_ASSERTION_H_INCLUDED

#include <exception>

#define DBC_ASSERT_STRINGIFY(n) #n
#define DBC_ASSERT_TOSTRING(n) DBC_ASSERT_STRINGIFY(n)

#define DBC_ASSERT(x)\
	(x) ? \
		(void)0 :\
		throw DesignByContract::Error(__FILE__ "(" DBC_ASSERT_TOSTRING(__LINE__) "): Assertion failed (" #x ")")

#define DBC_PRE(x)\
	(x) ? \
		(void)0 :\
		throw DesignByContract::Error(__FILE__ "(" DBC_ASSERT_TOSTRING(__LINE__) "): Pre-condition failed (" #x ")")

#define DBC_POST(x)\
	(x) ? \
		(void)0 :\
		throw DesignByContract::Error(__FILE__ "(" DBC_ASSERT_TOSTRING(__LINE__) "): Post-condition failed (" #x ")")

namespace DesignByContract {

class Error : public std::exception {
public:
	Error(const char* msg) : m_msg(msg) {}
	const char* what() const throw()
	{
		return m_msg;
	}
private:
	const char* m_msg;
};

}

#endif // DESIGNBYCONTRACT_ASSERTION_H_INCLUDED
