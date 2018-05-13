#ifndef ASSERTION_ASSERTION_H_INCLUDED
#define ASSERTION_ASSERTION_H_INCLUDED

#define ASSERTION_ASSERT_STRINGIFY(n) #n
#define ASSERTION_ASSERT_TOSTRING(n) ASSERTION_ASSERT_STRINGIFY(n)

#define CHECK_ASSERT(x)\
	(x) ? \
		(void)0 :\
		throw Assertion::Error(__FILE__ "(" ASSERTION_ASSERT_TOSTRING(__LINE__) "): Assertion failed (" #x ")")

#define CHECK_PRECOND(x)\
	(x) ? \
		(void)0 :\
		throw Assertion::Error(__FILE__ "(" ASSERTION_ASSERT_TOSTRING(__LINE__) "): Pre-condition failed (" #x ")")

#define CHECK_POSTCOND(x)\
	(x) ? \
		(void)0 :\
		throw Assertion::Error(__FILE__ "(" ASSERTION_ASSERT_TOSTRING(__LINE__) "): Post-condition failed (" #x ")")

namespace Assertion {

class Error {
public:
	explicit Error(const char* msg) : m_msg(msg) {}
	const char* message() const
	{
		return m_msg;
	}
private:
	const char* m_msg;
};

}

#endif // ASSERTION_ASSERTION_H_INCLUDED
